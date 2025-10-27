#include "jps.hpp"
#include "tools.hpp"

#include <float.h>

#define JPS_MAX_NEIGHBOURS 8

struct PQElement {
    double priority;
    Location loc;
};

struct PriorityQueue {
    PQElement* elements;
    int size;
    int capacity;
};

static PriorityQueue g_priority_queue = {0, 0, 0};

struct JpsBuffers {
    Location* came_from;
    double* cost_so_far;
    unsigned char* closed_set;
    int capacity;
};

static JpsBuffers g_jps_buffers = {0, 0, 0, 0};

struct BufferGuard
{
    Location* came_from;
    double* cost_so_far;
    unsigned char* closed_set;

    BufferGuard()
        : came_from(0)
        , cost_so_far(0)
        , closed_set(0)
    {
    }

    ~BufferGuard()
    {
        if(closed_set != 0) {
            delete[] closed_set;
        }
        if(cost_so_far != 0) {
            delete[] cost_so_far;
        }
        if(came_from != 0) {
            delete[] came_from;
        }
    }

    void release()
    {
        came_from = 0;
        cost_so_far = 0;
        closed_set = 0;
    }
};

static void pq_swap(PQElement* a, PQElement* b)
{
    PQElement temp = *a;
    *a = *b;
    *b = temp;
}

static void pq_ensure_capacity(PriorityQueue* pq, int capacity)
{
    if(capacity <= pq->capacity) {
        return;
    }

    PQElement* new_elements = new PQElement[capacity];
    int i;
    for(i = 0; i < pq->size; ++i) {
        new_elements[i] = pq->elements[i];
    }
    if(pq->elements != 0) {
        delete[] pq->elements;
    }
    pq->elements = new_elements;
    pq->capacity = capacity;
}

static void pq_sift_up(PriorityQueue* pq, int index)
{
    while(index > 0) {
        int parent = (index - 1) / 2;
        if(pq->elements[index].priority < pq->elements[parent].priority) {
            pq_swap(&pq->elements[index], &pq->elements[parent]);
            index = parent;
        }
        else {
            break;
        }
    }
}

static void pq_sift_down(PriorityQueue* pq, int index)
{
    while(1) {
        int left = index * 2 + 1;
        int right = left + 1;
        int smallest = index;

        if(left < pq->size && pq->elements[left].priority < pq->elements[smallest].priority) {
            smallest = left;
        }
        if(right < pq->size && pq->elements[right].priority < pq->elements[smallest].priority) {
            smallest = right;
        }

        if(smallest != index) {
            pq_swap(&pq->elements[index], &pq->elements[smallest]);
            index = smallest;
        }
        else {
            break;
        }
    }
}

static void pq_reset(PriorityQueue* pq)
{
    pq->size = 0;
}

static void pq_push(PriorityQueue* pq, double priority, const Location& loc)
{
    if(pq->size >= pq->capacity) {
        int new_capacity = (pq->capacity == 0) ? 16 : pq->capacity * 2;
        pq_ensure_capacity(pq, new_capacity);
    }

    pq->elements[pq->size].priority = priority;
    pq->elements[pq->size].loc = loc;
    pq->size += 1;
    pq_sift_up(pq, pq->size - 1);
}

static int pq_is_empty(const PriorityQueue* pq)
{
    return pq->size == 0;
}

static void pq_pop(PriorityQueue* pq, double* out_priority, Location* out_loc)
{
    if(pq->size == 0) {
        if(out_priority) { *out_priority = 0.0; }
        if(out_loc) { out_loc->x = 0; out_loc->y = 0; }
        return;
    }

    if(out_priority) { *out_priority = pq->elements[0].priority; }
    if(out_loc) { *out_loc = pq->elements[0].loc; }

    pq->size -= 1;
    if(pq->size > 0) {
        pq->elements[0] = pq->elements[pq->size];
        pq_sift_down(pq, 0);
    }
}

static void ensure_jps_buffers(int required)
{
    if(required <= g_jps_buffers.capacity) {
        return;
    }

    BufferGuard guard;
    guard.came_from = new Location[required];
    guard.cost_so_far = new double[required];
    guard.closed_set = new unsigned char[required];

    if(g_jps_buffers.came_from != 0) {
        delete[] g_jps_buffers.came_from;
    }
    if(g_jps_buffers.cost_so_far != 0) {
        delete[] g_jps_buffers.cost_so_far;
    }
    if(g_jps_buffers.closed_set != 0) {
        delete[] g_jps_buffers.closed_set;
    }

    g_jps_buffers.came_from = guard.came_from;
    g_jps_buffers.cost_so_far = guard.cost_so_far;
    g_jps_buffers.closed_set = guard.closed_set;
    g_jps_buffers.capacity = required;

    guard.release();
}

static void reset_jps_buffers(int size)
{
    int i;
    for(i = 0; i < size; ++i) {
        g_jps_buffers.came_from[i] = NoneLoc;
        g_jps_buffers.cost_so_far[i] = DBL_MAX;
        g_jps_buffers.closed_set[i] = 0;
    }
}

void jps_shutdown()
{
    if(g_priority_queue.elements != 0) {
        delete[] g_priority_queue.elements;
        g_priority_queue.elements = 0;
        g_priority_queue.size = 0;
        g_priority_queue.capacity = 0;
    }

    if(g_jps_buffers.came_from != 0) {
        delete[] g_jps_buffers.came_from;
        g_jps_buffers.came_from = 0;
    }
    if(g_jps_buffers.cost_so_far != 0) {
        delete[] g_jps_buffers.cost_so_far;
        g_jps_buffers.cost_so_far = 0;
    }
    if(g_jps_buffers.closed_set != 0) {
        delete[] g_jps_buffers.closed_set;
        g_jps_buffers.closed_set = 0;
    }
    g_jps_buffers.capacity = 0;
}

Location jump(const Grid& grid, const Location initial, const Location dir,
    const Location goal)
{
    Location current = initial;

    while(1) {
        Location new_loc = current + dir;
        if(!grid.valid_move(current, dir)) {
            return NoneLoc;
        }

        if(new_loc == goal) {
            return new_loc;
        }

        Location forced_neighbours[JPS_MAX_NEIGHBOURS];
        int forced_count = grid.pruned_neighbours(new_loc, current, forced_neighbours, JPS_MAX_NEIGHBOURS);
        int i;
        for(i = 0; i < forced_count; ++i) {
            const Location& next = forced_neighbours[i];
            if(grid.forced(next, new_loc, dir)) {
                return new_loc;
            }
        }

        if(dir.x != 0 && dir.y != 0) {
            Location new_dirs[2];
            new_dirs[0] = make_location(dir.x, 0);
            new_dirs[1] = make_location(0, dir.y);
            for(i = 0; i < 2; ++i) {
                Location jump_point = jump(grid, new_loc, new_dirs[i], goal);
                if(jump_point != NoneLoc) {
                    return new_loc;
                }
            }
        }

        current = new_loc;
    }
}

int successors(const Grid& grid, const Location& current,
    const Location& parent, const Location& goal,
    Location* out, int max_count)
{
    Location neighbour_list[JPS_MAX_NEIGHBOURS];
    int neighbour_count = grid.pruned_neighbours(current, parent, neighbour_list, JPS_MAX_NEIGHBOURS);

    int out_count = 0;
    int i;
    for(i = 0; i < neighbour_count; ++i) {
        const Location& n = neighbour_list[i];
        Location direction = (n - current).direction();
        Location jump_point = jump(grid, current, direction, goal);
        if(jump_point != NoneLoc && out_count < max_count) {
            out[out_count] = jump_point;
            out_count += 1;
        }
    }

    return out_count;
}

static int reconstruct_path(
    const Grid& grid,
    const Location& start,
    const Location& goal,
    const Location* came_from,
    Location* out_path,
    int max_path_length)
{
    Location current = goal;
    int count = 0;

    while(1) {
        if(count >= max_path_length) {
            return -1;
        }

        out_path[count] = current;
        count += 1;

        if(current == start) {
            break;
        }

        int index = grid.to_index(current);
        Location parent = came_from[index];
        if(parent == NoneLoc) {
            return -1;
        }
        current = parent;
    }

    int i;
    for(i = 0; i < count / 2; ++i) {
        Location temp = out_path[i];
        out_path[i] = out_path[count - 1 - i];
        out_path[count - 1 - i] = temp;
    }

    return count;
}

int jps_find_path(
    const Grid& grid,
    const Location& start, const Location& goal,
    heuristic_fn heuristic,
    Location* out_path,
    int max_path_length)
{
    const int grid_size = grid.grid_size();

    ensure_jps_buffers(grid_size);
    reset_jps_buffers(grid_size);

    pq_reset(&g_priority_queue);

    int start_idx = grid.to_index(start);
    g_jps_buffers.came_from[start_idx] = start;
    g_jps_buffers.cost_so_far[start_idx] = 0.0;

    pq_push(&g_priority_queue, 0.0, start);

    Location parent = NoneLoc;

    while(!pq_is_empty(&g_priority_queue)) {
        double current_priority;
        Location current;
        pq_pop(&g_priority_queue, &current_priority, &current);

        int current_idx = grid.to_index(current);
        if(g_jps_buffers.closed_set[current_idx]) {
            continue;
        }
        g_jps_buffers.closed_set[current_idx] = 1;

        if(current == goal) {
            int path_len = reconstruct_path(grid, start, goal, g_jps_buffers.came_from, out_path, max_path_length);
            return path_len;
        }

        if(current != start) {
            parent = g_jps_buffers.came_from[current_idx];
        }
        else {
            parent = NoneLoc;
        }

        Location next_nodes[JPS_MAX_NEIGHBOURS];
        int next_count = successors(grid, current, parent, goal, next_nodes, JPS_MAX_NEIGHBOURS);

        int i;
        for(i = 0; i < next_count; ++i) {
            const Location& next = next_nodes[i];
            int next_idx = grid.to_index(next);

            if(g_jps_buffers.closed_set[next_idx]) {
                continue;
            }

            double new_cost = g_jps_buffers.cost_so_far[current_idx] + heuristic(current, next);
            double existing_cost = g_jps_buffers.cost_so_far[next_idx];

            if(existing_cost == DBL_MAX || new_cost < existing_cost) {
                g_jps_buffers.cost_so_far[next_idx] = new_cost;
                g_jps_buffers.came_from[next_idx] = current;
                double priority = new_cost + heuristic(next, goal);
                pq_push(&g_priority_queue, priority, next);
            }
        }
    }

    return -1;
}
