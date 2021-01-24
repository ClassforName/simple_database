#include"Node.h"

static const uint32_t ID_SIZE = size_of_attribute(Row, id);
static const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
static const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
static const uint32_t ID_OFFSET = 0;
static const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
static const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
static const uint32_t ROW_SIZE = ID_SIZE + EMAIL_SIZE + USERNAME_SIZE;

const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint32_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_OFFSET + NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint32_t COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_OFFSET;

const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;

uint32_t get_leaf_node_max_cells(){
    return LEAF_NODE_MAX_CELLS; 
}
uint32_t *leaf_node_num_cells(void *node){
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

uint32_t *leaf_node_cell(void *node, uint32_t cell_nums){
    return node + LEAF_NODE_HEADER_SIZE + cell_nums * LEAF_NODE_CELL_SIZE;
}

uint32_t *leaf_node_key(void *node, uint32_t cell_nums){
    return leaf_node_cell(node, cell_nums);
}

uint32_t *leaf_node_value(void* node, uint32_t cell_nums){
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
        printf("Need to spilit a leaf node \n");
        exit(EXIT_FAILURE);
    }
    if(cursor->cell_num < num_cell){
        for(int i = num_cell; i > cursor->cell_num; i--){
            memcpy((void*)leaf_node_cell(node, i), (void*)leaf_node_cell(node, i-1), LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, cursor->cell_num)) = key;
    serialize_row(value, leaf_node_value(node, cursor->cell_num));
}

void print_constant(){
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADR_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_NUM_CELLS_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELL: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELL: %d\n", LEAF_NODE_MAX_CELLS);
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