#include"Node.h"

#define ID_SIZE (size_of_attribute(Row, id))
#define USERNAME_SIZE (size_of_attribute(Row, username))
#define EMAIL_SIZE (size_of_attribute(Row, email))
#define ID_OFFSET 0
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)
#define ROW_SIZE (ID_SIZE + EMAIL_SIZE + USERNAME_SIZE)

#define NODE_TYPE_SIZE (sizeof(uint8_t))
#define NODE_TYPE_OFFSET 0
#define IS_ROOT_SIZE (sizeof(uint32_t))
#define IS_ROOT_OFFSET (NODE_TYPE_OFFSET + NODE_TYPE_SIZE)
#define PARENT_POINTER_SIZE (sizeof(uint32_t))
#define PARENT_POINTER_OFFSET (IS_ROOT_OFFSET + IS_ROOT_SIZE)
#define COMMON_NODE_HEADER_SIZE (NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE)

#define LEAF_NODE_NUM_CELLS_SIZE (sizeof(uint32_t))
#define LEAF_NODE_NUM_CELLS_OFFSET (COMMON_NODE_HEADER_SIZE)
#define LEAF_NODE_HEADER_SIZE (COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE)

#define LEAF_NODE_KEY_SIZE (sizeof(uint32_t))
#define LEAF_NODE_KEY_OFFSET (0)
#define LEAF_NODE_VALUE_SIZE (ROW_SIZE)
#define LEAF_NODE_VALUE_OFFSET (LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE)
#define LEAF_NODE_CELL_SIZE (LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE)
#define LEAF_NODE_SPACE_FOR_CELLS (PAGE_SIZE - LEAF_NODE_HEADER_SIZE)
#define LEAF_NODE_MAX_CELLS (LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE)

#define LEAF_NODE_RIGHT_SPLIT_COUNT (LEAF_NODE_MAX_CELLS / 2)
#define LEAF_NODE_LEFT_SPLIT_COUNT (LEAF_NODE_MAX_CELLS + 1 - LEAF_NODE_RIGHT_SPLIT_COUNT)

#define INTERNAL_NODE_NUM_KEYS_SIZE (sizeof(uint32_t))
#define INTERNAL_NODE_NUM_KEYS_OFFSET COMMON_NODE_HEADER_SIZE
#define INTERNAL_NODE_RIGHT_CHILD_SIZE (sizeof(uint32_t))
#define INTERNAL_NDOE_RIGHT_CHILD_OFFSET (INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE)
#define INTERNAL_NODE_HEADER_SIZE (COMMON_NODE_HEADER_SIZE + INTERNAL_NODE_NUM_KEYS_SIZE + INTERNAL_NODE_RIGHT_CHILD_SIZE)

#define INTERNAL_NODE_KEY_SIZE (sizeof(uint32_t))
#define INTERNAL_NODE_CHILD_SIZE (sizeof(uint32_t))
#define INTERNAL_NODE_CELL_SIZE (INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE)
uint32_t get_leaf_node_max_cells(){
    return LEAF_NODE_MAX_CELLS; 
}
uint32_t *leaf_node_num_cells(void *node){
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void *leaf_node_cell(void *node, uint32_t cell_nums){
    return node + LEAF_NODE_HEADER_SIZE + cell_nums * LEAF_NODE_CELL_SIZE;
}

uint32_t *leaf_node_key(void *node, uint32_t cell_nums){
    return (uint32_t*)leaf_node_cell(node, cell_nums);
}

void *leaf_node_value(void* node, uint32_t cell_nums){
    return leaf_node_cell(node, cell_nums) + LEAF_NODE_KEY_SIZE;
}

void initialize_leaf_node(void *node){
    *leaf_node_num_cells(node) = 0;
    set_node_type(node, NODE_LEAF);
}

void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value)
{
    void *node = get_page(cursor->table->pager, cursor->page_num);
    uint32_t num_cell = *leaf_node_num_cells(node);
    if(num_cell > LEAF_NODE_MAX_CELLS){
        leaf_node_split_and_insert(cursor, key, value);
    }
    if(cursor->cell_num < num_cell){
        for(int i = num_cell; i > cursor->cell_num; i--){
            memmove((void*)leaf_node_cell(node, i), (void*)leaf_node_cell(node, i-1), LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor->cell_num)) = key;
    serialize_row(value, leaf_node_value(node, cursor->cell_num));
}

void print_constant(){
    printf("ROW_SIZE: %ld\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %ld\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADR_SIZE: %ld\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %ld\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELL: %ld\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELL: %ld\n", LEAF_NODE_MAX_CELLS);
}

void print_leaf_node(void *node){
  uint32_t cell_num = *leaf_node_num_cells(node);
  printf("leaf(size %d)\n", cell_num);
  for(uint32_t i = 0; i < cell_num; i++){
    uint32_t key = *leaf_node_key(node, i);
    printf("- %d : %d\n", i, key); 
  }
}

NodeType get_node_type(void *node){
    uint8_t node_type = *((uint8_t *)(node + NODE_TYPE_OFFSET));
    return (NodeType)node_type;
}

void set_node_type(void *node, NodeType node_type){
   *((uint8_t*)(node + NODE_TYPE_OFFSET)) = node_type; 
}

static void leaf_node_split_and_insert(Cursor* cursor, uint32_t key, Row* value)
{
    void *old_node = get_page(cursor->table->pager, cursor->page_num);
    uint32_t new_page_num = get_unused_page_num(cursor->table->pager);
    void *new_node = get_page(cursor->table->pager, new_page_num);
    initialize_leaf_node(new_node);

    for(int i = LEAF_NODE_MAX_CELLS; i >= 0; i--){
        void* destination_node = old_node;
        if(i >= LEAF_NODE_LEFT_SPLIT_COUNT){
            destination_node = new_node;
        }
        
        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void* destination = leaf_node_cell(destination_node, index_within_node);
        if(i == cursor->cell_num){
            serialize_row(value, destination);
        }else if(i < cursor->cell_num){
            memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
        }else{
            memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    if(is_node_root(old_node)){
        return create_new_root(cursor->table, new_page_num);
    }else{
        printf("Need to implement updating parent after spilit\n");
        exit(EXIT_FAILURE);
    }
}

static uint32_t get_unused_page_num(Pager* pager)
{
    return pager->nums_page;
}

static bool is_node_root(void *node)
{
    return get_node_type(node) == NODE_INTERNAL;
}

static void create_new_root(Table *table, uint32_t right_page_num)
{
    void *root = get_page(table->pager, table->root_page_num);
    void *right_child = get_page(table->pager, right_page_num);
    uint32_t left_page_num = get_unused_page_num(table->pager);
    void *left_child = get_page(table->pager, left_page_num);

    memcpy(left_child, root, PAGE_SIZE);
    return;
}

static uint32_t* internal_node_num_key(void *node)
{
    return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

static uint32_t* internal_node_right_child(void *node)
{
    return node + INTERNAL_NDOE_RIGHT_CHILD_OFFSET;
}

static uint32_t* internal_node_cell(void *node, uint32_t cell_num)
{
    return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

static uint32_t* internal_node_child(void *node, uint32_t child_num)
{
    uint32_t num_key = *internal_node_num_key(node);
    if(child_num > num_key){
        printf("Tried to access child_num %d > nums_key %d\n", child_num, num_key);
        exit(EXIT_FAILURE);
    }else if(child_num == num_key){
        return internal_node_right_child(node);
    }else{
        return internal_node_cell(node, child_num);
    }
}
