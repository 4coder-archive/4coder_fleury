
//~ NOTE(rjf): Calc Comments

typedef enum CalcTokenType CalcTokenType;
enum CalcTokenType
{
    CALC_TOKEN_TYPE_invalid,
    CALC_TOKEN_TYPE_identifier,
    CALC_TOKEN_TYPE_source_code_identifier,
    CALC_TOKEN_TYPE_number,
    CALC_TOKEN_TYPE_symbol,
    CALC_TOKEN_TYPE_string_constant,
};

typedef struct CalcToken CalcToken;
struct CalcToken
{
    CalcTokenType type;
    char *string;
    int string_length;
};

static CalcToken
Fleury4GetNextCalcToken(char *buffer)
{
    CalcToken token = {0};
    
    if(buffer)
    {
        for(int i = 0; buffer[i]; ++i)
        {
            if(buffer[i] == '@')
            {
                token.type = CALC_TOKEN_TYPE_source_code_identifier;
                token.string = buffer+i+1;
                int j;
                for(j = i+1; buffer[j] &&
                    (CharIsDigit(buffer[j]) || buffer[j] == '_' ||
                     CharIsAlpha(buffer[j]));
                    ++j);
                token.string_length = j - i - 1;
                break;
            }
            else if(CharIsAlpha(buffer[i]))
            {
                token.type = CALC_TOKEN_TYPE_identifier;
                token.string = buffer+i;
                int j;
                for(j = i+1; buffer[j] &&
                    (CharIsDigit(buffer[j]) || buffer[j] == '_' ||
                     CharIsAlpha(buffer[j]));
                    ++j);
                token.string_length = j - i;
                break;
            }
            else if(CharIsDigit(buffer[i]))
            {
                token.type = CALC_TOKEN_TYPE_number;
                token.string = buffer+i;
                int j;
                for(j = i+1; buffer[j] &&
                    (CharIsDigit(buffer[j]) || buffer[j] == '.' ||
                     CharIsAlpha(buffer[j]));
                    ++j);
                token.string_length = j - i;
                break;
            }
            else if(CharIsSymbol(buffer[i]))
            {
                token.type = CALC_TOKEN_TYPE_symbol;
                token.string = buffer+i;
                
                // NOTE(rjf): Assumes 1-length symbols. Might not always be true.
                int j = i+1;
                // for(j = i+1; buffer[j] && CharIsSymbol(buffer[j]); ++j);
                
                token.string_length = j - i;
                break;
            }
            else if(buffer[i] == '"' || buffer[i] == '\'')
            {
                int starting_char = buffer[i];
                token.type = CALC_TOKEN_TYPE_string_constant;
                token.string = buffer+i;
                int j;
                for(j = i+1; buffer[j] && buffer[j] != starting_char; ++j);
                token.string_length = j - i + 1;
                break;
            }
        }
    }
    
    return token;
}

static CalcToken
Fleury4NextCalcToken(char **at)
{
    CalcToken token = Fleury4GetNextCalcToken(*at);
    *at = token.string + token.string_length;
    return token;
}

static CalcToken
Fleury4PeekCalcToken(char **at)
{
    CalcToken token = Fleury4GetNextCalcToken(*at);
    return token;
}

static int
Fleury4CalcTokenMatch(CalcToken token, char *str)
{
    int match = 0;
    
    if(token.string && token.string_length > 0 &&
       token.type != CALC_TOKEN_TYPE_invalid)
    {
        match = 1;
        for(int i = 0; i < token.string_length; ++i)
        {
            if(token.string[i] == str[i])
            {
                if(i == token.string_length-1)
                {
                    if(str[i+1] != 0)
                    {
                        match = 0;
                        break;
                    }
                }
            }
            else
            {
                match = 0;
                break;
            }
        }
    }
    return match;
}

static int
Fleury4RequireCalcToken(char **at, char *str)
{
    int result = 0;
    CalcToken token = Fleury4GetNextCalcToken(*at);
    if(Fleury4CalcTokenMatch(token, str))
    {
        result = 1;
        *at = token.string + token.string_length;
    }
    return result;
}

static int
Fleury4RequireCalcTokenType(char **at, CalcTokenType type, CalcToken *token_ptr)
{
    int result = 0;
    CalcToken token = Fleury4GetNextCalcToken(*at);
    if(token.type == type)
    {
        result = 1;
        *at = token.string + token.string_length;
        if(token_ptr)
        {
            *token_ptr = token;
        }
    }
    return result;
}

static int
Fleury4RequireNewline(char **at)
{
    int result = 0;
    
    CalcToken next_token = Fleury4PeekCalcToken(at);
    char *newline = 0;
    for(int i = 0; (*at)[i]; ++i)
    {
        if((*at)[i] == '\n')
        {
            newline = (*at)+i;
            break;
        }
    }
    
    if(newline)
    {
        if(next_token.string > newline)
        {
            result = 1;
        }
    }
    
    return result;
}


//~ NOTE(rjf): Calc node-types and types.

#define CALC_NODE_TYPE_LIST                \
CalcNodeType(invalid,                0)\
CalcNodeType(number,                 0)\
CalcNodeType(string_constant,        0)\
CalcNodeType(array,                  0)\
CalcNodeType(identifier,             0)\
CalcNodeType(source_code_identifier, 0)\
CalcNodeType(function_call,          0)\
CalcNodeType(add,                    1)\
CalcNodeType(subtract,               1)\
CalcNodeType(multiply,               2)\
CalcNodeType(divide,                 2)\
CalcNodeType(raise_to_power,         0)\
CalcNodeType(negate,                 0)\
CalcNodeType(assignment,             0)

#define CALC_TYPE_LIST                                      \
CalcType(error,                 "error")                \
CalcType(none,                  "none")                 \
CalcType(number,                "number")               \
CalcType(array,                 "array")                \
CalcType(string,                "string")               \
CalcType(source_code_reference, "source code reference")



//~
typedef enum CalcNodeType CalcNodeType;
enum CalcNodeType
{
#define CalcNodeType(name, precedence) CALC_NODE_TYPE_##name,
    CALC_NODE_TYPE_LIST
    #undef CalcNodeType
};

static int
Fleury4CalcOperatorPrecedence(CalcNodeType type)
{
    static int precedence_table[] =
    {
#define CalcNodeType(name, precedence) precedence,
        CALC_NODE_TYPE_LIST
    #undef CalcNodeType
    };
    return precedence_table[type];
}

typedef enum CalcType CalcType;
enum CalcType
{
#define CalcType(name, str) CALC_TYPE_##name,
    CALC_TYPE_LIST
    #undef CalcType
};

static char *
Fleury4CalcTypeName(CalcType type)
{
    static char *name_table[] =
    {
#define CalcType(name, str) str,
        CALC_TYPE_LIST
    #undef CalcType
    };
    return name_table[type];
}

//~



typedef struct CalcNode CalcNode;
struct CalcNode
{
    CalcNodeType type;
    double value;
    union
    {
        CalcNode *operand;
        CalcNode *left;
    };
    CalcNode *right;
    union
    {
        CalcNode *first_parameter;
        CalcNode *first_member;
    };
    CalcNode *next;
    CalcToken token;
    int num_params;
};

static CalcNode *
AllocateCalcNode(MemoryArena *arena, CalcNodeType type)
{
    CalcNode *node = (CalcNode *)MemoryArenaAllocate(arena, sizeof(*node));
    MemorySet(node, 0, sizeof(*node));
    node->type = type;
    return node;
}

static CalcNode *Fleury4ParseCalcExpression(MemoryArena *arena, char **at_ptr);

static CalcNode *
Fleury4ParseCalcUnaryExpression(MemoryArena *arena, char **at_ptr)
{
    CalcNode *expression = 0;
    
    CalcToken token = Fleury4PeekCalcToken(at_ptr);
    
    if(Fleury4CalcTokenMatch(token, "-"))
    {
        Fleury4NextCalcToken(at_ptr);
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_negate);
        expression->operand = Fleury4ParseCalcUnaryExpression(arena, at_ptr);
    }
    else if(token.type == CALC_TOKEN_TYPE_source_code_identifier)
    {
        Fleury4NextCalcToken(at_ptr);
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_source_code_identifier);
        expression->token = token;
    }
    else if(token.type == CALC_TOKEN_TYPE_identifier)
    {
        Fleury4NextCalcToken(at_ptr);
        
        // NOTE(rjf): Function call.
        if(Fleury4RequireCalcToken(at_ptr, "("))
        {
            expression = AllocateCalcNode(arena, CALC_NODE_TYPE_function_call);
            expression->token = token;
            
            CalcNode **target_param = &expression->first_parameter;
            for(;;)
            {
                CalcToken next_token = Fleury4PeekCalcToken(at_ptr);
                if(next_token.type == CALC_TOKEN_TYPE_invalid ||
                   Fleury4CalcTokenMatch(next_token, ")"))
                {
                    break;
                }
                
                CalcNode *param = Fleury4ParseCalcExpression(arena, at_ptr);
                
                if(param)
                {
                    *target_param = param;
                    target_param = &(*target_param)->next;
                    Fleury4RequireCalcToken(at_ptr, ",");
                }
                else
                {
                    expression = 0;
                    goto end_parse;
                }
            }
            
            if(!Fleury4RequireCalcToken(at_ptr, ")"))
            {
                expression = 0;
                goto end_parse;
            }
        }
        
        // NOTE(rjf): Constant or variable.
        else
        {
            expression = AllocateCalcNode(arena, CALC_NODE_TYPE_identifier);
            expression->token = token;
        }
    }
    else if(Fleury4CalcTokenMatch(token, "("))
    {
        Fleury4NextCalcToken(at_ptr);
        expression = Fleury4ParseCalcExpression(arena, at_ptr);
        Fleury4RequireCalcToken(at_ptr, ")");
    }
    else if(token.type == CALC_TOKEN_TYPE_number)
    {
        Fleury4NextCalcToken(at_ptr);
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_number);
        expression->value = GetFirstDoubleFromBuffer(token.string);
    }
    else if(token.type == CALC_TOKEN_TYPE_string_constant)
    {
        Fleury4NextCalcToken(at_ptr);
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_string_constant);
        expression->token = token;
    }
    else if(Fleury4CalcTokenMatch(token, "["))
    {
        Fleury4NextCalcToken(at_ptr);
        
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_array);
        CalcNode **target_member = &expression->first_member;
        
        for(;;)
        {
            token = Fleury4PeekCalcToken(at_ptr);
            if(Fleury4CalcTokenMatch(token, "]") || token.type == CALC_TOKEN_TYPE_invalid)
            {
                break;
            }
            
            *target_member = Fleury4ParseCalcExpression(arena, at_ptr);
            target_member = &(*target_member)->next;
            
            while(Fleury4RequireCalcToken(at_ptr, ","));
        }
        
        Fleury4RequireCalcToken(at_ptr, "]");
    }
    
    if(Fleury4RequireCalcToken(at_ptr, "^"))
    {
        CalcNode *old_expr = expression;
        expression = AllocateCalcNode(arena, CALC_NODE_TYPE_raise_to_power);
        expression->left = old_expr;
        expression->right = Fleury4ParseCalcUnaryExpression(arena, at_ptr);
    }
    
    end_parse:;
    return expression;
}

static CalcNodeType
Fleury4GetCalcBinaryOperatorTypeFromToken(CalcToken token)
{
    CalcNodeType type = CALC_NODE_TYPE_invalid;
    switch(token.type)
    {
        case CALC_TOKEN_TYPE_symbol:
        {
            if(token.string[0] == '+')
            {
                type = CALC_NODE_TYPE_add;
            }
            else if(token.string[0] == '-')
            {
                type = CALC_NODE_TYPE_subtract;
            }
            else if(token.string[0] == '*')
            {
                type = CALC_NODE_TYPE_multiply;
            }
            else if(token.string[0] == '/')
            {
                type = CALC_NODE_TYPE_divide;
            }
            break;
        }
        default: break;
    }
    return type;
}

static CalcNode *
Fleury4ParseCalcExpression_(MemoryArena *arena, char **at_ptr, int precedence_in)
{
    CalcNode *expression = Fleury4ParseCalcUnaryExpression(arena, at_ptr);
    
    if(!expression)
    {
        goto end_parse;
    }
    
    CalcToken token = Fleury4PeekCalcToken(at_ptr);
    CalcNodeType operator_type = Fleury4GetCalcBinaryOperatorTypeFromToken(token);
    
    if(token.string && operator_type != CALC_NODE_TYPE_invalid &&
       operator_type != CALC_NODE_TYPE_number)
    {
        for(int precedence = Fleury4CalcOperatorPrecedence(operator_type);
            precedence >= precedence_in;
            --precedence)
        {
            for(;;)
            {
                token = Fleury4PeekCalcToken(at_ptr);
                
                operator_type = Fleury4GetCalcBinaryOperatorTypeFromToken(token);
                int operator_precedence = Fleury4CalcOperatorPrecedence(operator_type);
                
                if(operator_precedence != precedence)
                {
                    break;
                }
                
                if(operator_type == CALC_NODE_TYPE_invalid)
                {
                    break;
                }
                
                Fleury4NextCalcToken(at_ptr);
                
                CalcNode *right = Fleury4ParseCalcExpression_(arena, at_ptr, precedence+1);
                CalcNode *existing_expression = expression;
                expression = AllocateCalcNode(arena, operator_type);
                expression->type = operator_type;
                expression->left = existing_expression;
                expression->right = right;
                
                if(!right)
                {
                    goto end_parse;
                }
            }
        }
    }
    
    end_parse:;
    return expression;
}

static CalcNode *
Fleury4ParseCalcExpression(MemoryArena *arena, char **at_ptr)
{
    return Fleury4ParseCalcExpression_(arena, at_ptr, 1);
}

static CalcNode *
Fleury4ParseCalcCode(MemoryArena *arena, char **at_ptr)
{
    CalcNode *root = 0;
    CalcNode **target = &root;
    
    for(;;)
    {
        CalcToken token = Fleury4PeekCalcToken(at_ptr);
        
        // NOTE(rjf): Parse assignment.
        if(token.type == CALC_TOKEN_TYPE_identifier)
        {
            char *at_reset = *at_ptr;
            Fleury4NextCalcToken(at_ptr);
            
            // NOTE(rjf): Variable assignment
            if(Fleury4RequireCalcToken(at_ptr, "="))
            {
                CalcNode *identifier = AllocateCalcNode(arena, CALC_NODE_TYPE_identifier);
                identifier->token = token;
                
                CalcNode *assignment = AllocateCalcNode(arena, CALC_NODE_TYPE_assignment);
                assignment->left = identifier;
                assignment->right = Fleury4ParseCalcExpression(arena, at_ptr);
                
                if(assignment == 0)
                {
                    break;
                }
                
                *target = assignment;
                target = &(*target)->next;
                goto end_parse;
            }
            else
            {
                *at_ptr = at_reset;
            }
        }
        
        // NOTE(rjf): Parse expression.
        {
            CalcNode *expression = Fleury4ParseCalcExpression(arena, at_ptr);
            if(expression == 0)
            {
                break;
            }
            *target = expression;
            target = &(*target)->next;
            goto end_parse;
        }
        
        end_parse:;
        
        if(!Fleury4RequireCalcToken(at_ptr, ";") && !Fleury4RequireNewline(at_ptr))
        {
            break;
        }
    }
    
    return root;
}

typedef struct CalcSymbolKey CalcSymbolKey;
struct CalcSymbolKey
{
    char *string;
    int string_length;
};

typedef struct CalcSymbolValue CalcSymbolValue;
struct CalcSymbolValue
{
    CalcNode *node;
};

typedef struct CalcSymbolTable CalcSymbolTable;
struct CalcSymbolTable
{
    unsigned int size;
    CalcSymbolKey *keys;
    CalcSymbolValue *values;
};

typedef struct CalcInterpretGraph CalcInterpretGraph;
struct CalcInterpretGraph
{
    CalcNode *graph_expression;
    CalcNode *input_value;
    CalcNode *parent_call;
    char *plot_title;
    int plot_title_length;
    char *x_axis;
    int x_axis_length;
    char *y_axis;
    int y_axis_length;
    Rect_f32 plot_view;
    Plot2DMode mode;
    int num_function_samples;
    int num_bins;
    Range_f32 bin_data_range;
    CalcInterpretGraph *next;
};

typedef struct CalcValue CalcValue;
struct CalcValue
{
    union
    {
        struct
        {
            int string_length;
            char *as_string;
        };
        
        struct
        {
            int array_count;
            CalcValue *as_array;
        };
        
        struct
        {
            char *as_error;
        };
        
        struct
        {
            double as_f64;
        };
        
        struct
        {
            i64 as_token_offset;
        };
    };
    
    CalcType type;
};

static CalcValue
CalcValueNone(void)
{
    CalcValue calc_value = {0};
    calc_value.type = CALC_TYPE_none;
    return calc_value;
}

static CalcValue
CalcValueF64(double num)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_number;
    val.as_f64 = num;
    return val;
}

static CalcValue
CalcValueError(char *string)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_error;
    val.as_error = string;
    return val;
}

static CalcValue
CalcValueString(char *string, int string_length)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_string;
    val.as_string = string;
    val.string_length = string_length;
    return val;
}

static CalcValue
CalcValueSourceCodeReference(i64 token_position)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_source_code_reference;
    val.as_token_offset = token_position;
    return val;
}

typedef struct CalcInterpretResult CalcInterpretResult;
struct CalcInterpretResult
{
    CalcValue value;
    CalcInterpretGraph *first_graph;
};

typedef struct CalcInterpretContext CalcInterpretContext;
struct CalcInterpretContext
{
    Application_Links *app;
    Buffer_ID buffer;
    Text_Layout_ID text_layout_id;
    MemoryArena *arena;
    CalcSymbolTable *symbol_table;
    f32 current_time;
    
    // NOTE(rjf): Plot data.
    struct
    {
        char *plot_title;
        int plot_title_length;
        char *x_axis;
        int x_axis_length;
        char *y_axis;
        int y_axis_length;
        f32 x_low;
        f32 x_high;
        f32 y_low;
        f32 y_high;
        int num_function_samples;
        int num_bins;
        f32 bin_range_low;
        f32 bin_range_high;
    };
};

// NOTE(rjf): WHY DOESN'T C++ ALLOW DECLARING THINGS IN THE ORDER I WANT THIS SUCKS SO BAD
static CalcInterpretResult
Fleury4InterpretCalcExpression(CalcInterpretContext *context, CalcNode *root);

static CalcValue
CalcValueArray(CalcInterpretContext *context, CalcNode *first_member)
{
    CalcValue val = {0};
    val.type = CALC_TYPE_array;
    
    CalcType array_type = CALC_TYPE_none;
    int count = 0;
    
    for(CalcNode *member = first_member; member; member = member->next)
    {
        ++count;
    }
    
    // NOTE(rjf): WHY DOESN'T C++ ALLOW IMPLICIT POINTER CASTING THIS SUCKS SO BAD
    CalcValue *array = (CalcValue *)MemoryArenaAllocate(context->arena, count*sizeof(*array));
    
    int write_pos = 0;
    
    for(CalcNode *member = first_member; member; member = member->next)
    {
        CalcInterpretResult result = Fleury4InterpretCalcExpression(context, member);
        
        if(member == first_member)
        {
            array_type = result.value.type;
            if(array_type == CALC_TYPE_error)
            {
                val = result.value;
                goto end_create;
            }
            else if(array_type == CALC_TYPE_none)
            {
                val = CalcValueError("Cannot make arrays of 'none' type.");
                goto end_create;
            }
        }
        else
        {
            if(result.value.type != array_type)
            {
                val = CalcValueError("Cannot have multiple types in an array.");
                goto end_create;
            }
        }
        
        array[write_pos++] = result.value;
    }
    
    if(array && count)
    {
        val.as_array = array;
        val.array_count = count;
    }
    
    end_create:;
    return val;
}

static CalcSymbolTable
CalcSymbolTableInit(MemoryArena *arena, unsigned int size)
{
    CalcSymbolTable table = {0};
    table.size = size;
    table.keys = (CalcSymbolKey *)MemoryArenaAllocate(arena, sizeof(*table.keys)*size);
    table.values = (CalcSymbolValue *)MemoryArenaAllocate(arena, sizeof(*table.values)*size);
    MemorySet(table.keys, 0, sizeof(*table.keys)*size);
    MemorySet(table.values, 0, sizeof(*table.values)*size);
    return table;
}

static CalcSymbolValue *
CalcSymbolTableLookup_(CalcSymbolTable *table, char *string, int length)
{
    CalcSymbolValue *result = 0;
    
    unsigned int hash = StringCRC32(string, length) % table->size;
    unsigned int original_hash = hash;
    
    CalcSymbolValue *value = 0;
    
    for(;;)
    {
        if(table->keys[hash].string)
        {
            if(StringMatchCaseSensitive(table->keys[hash].string, table->keys[hash].string_length,
                                        string, length))
            {
                value = table->values + hash;
                break;
            }
            else
            {
                if(++hash >= table->size)
                {
                    hash = 0;
                }
                if(hash == original_hash)
                {
                    break;
                }
            }
        }
        else
        {
            break;
        }
    }
    
    if(value)
    {
        result = value;
    }
    
    return result;
}

static CalcNode *
CalcSymbolTableLookup(CalcSymbolTable *table, char *string, int string_length)
{
    CalcNode *result = 0;
    CalcSymbolValue *value = CalcSymbolTableLookup_(table, string, string_length);
    if(value)
    {
        result = value->node;
    }
    return result;
}

static void
CalcSymbolTableAdd(CalcSymbolTable *table, char *string, int string_length, CalcNode *node)
{
    unsigned int hash = StringCRC32(string, string_length) % table->size;
    unsigned int original_hash = hash;
    int found = 0;
    
    for(;;)
    {
        if(table->keys[hash].string)
        {
            if(StringMatchCaseSensitive(table->keys[hash].string, table->keys[hash].string_length,
                                        string, string_length))
            {
                found = 1;
                break;
            }
            else
            {
                if(++hash >= table->size)
                {
                    hash = 0;
                }
                if(hash == original_hash)
                {
                    break;
                }
            }
        }
        else
        {
            found = 1;
            break;
        }
    }
    
    if(found)
    {
        table->keys[hash].string = string;
        table->keys[hash].string_length = string_length;
        table->values[hash].node = node;
    }
}

static void
Fleury4GetDataFromSourceCode(Application_Links *app, Buffer_ID buffer, Text_Layout_ID text_layout_id,
                             i64 start_pos, MemoryArena *arena, float **data_ptr, int *data_count_ptr)
{
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    
    if(token_array.tokens != 0)
    {
        Token_Iterator_Array it = token_iterator_pos(0, &token_array, start_pos);
        Token *token = 0;
        
        // NOTE(rjf): Find scope open (opening brace of initializer).
        for(;;)
        {
            token = token_it_read(&it);
            if(token->pos >= start_pos + 30 || !token ||
               !token_it_inc_non_whitespace(&it))
            {
                goto end_read_data;
            }
            
            if(token->kind == TokenBaseKind_ScopeOpen)
            {
                break;
            }
        }
        
        // NOTE(rjf): Read data.
        float *data = (float *)MemoryArenaAllocate(arena, 0);
        int data_count = 0;
        b32 is_negative = 0;
        for(;;)
        {
            token = token_it_read(&it);
            if(!token || !token_it_inc_non_whitespace(&it))
            {
                goto end_read_data;
            }
            
            if(token->kind == TokenBaseKind_Operator &&
               token->sub_kind == TokenCppKind_Minus)
            {
                is_negative = 1;
            }
            
            if(token->kind == TokenBaseKind_LiteralFloat ||
               token->kind == TokenBaseKind_LiteralInteger)
            {
                Range_i64 token_range =
                {
                    token->pos,
                    token->pos + (token->size > 256 ? 256 : token->size),
                };
                
                u8 token_buffer[256];
                buffer_read_range(app, buffer, token_range, token_buffer);
                
                float sign = is_negative ? -1.f : 1.f;
                is_negative = 0;
                
                MemoryArenaAllocate(arena, sizeof(data[0]));
                data[data_count++] = sign * (float)GetFirstDoubleFromBuffer((char *)token_buffer);
            }
            else if(token->kind == TokenBaseKind_ScopeClose)
            {
                break;
            }
        }
        
        *data_ptr = data;
        *data_count_ptr = data_count;
        
        end_read_data:;
        
    }
}

static void
Fleury4GraphCalcExpression(Application_Links *app, Face_ID face_id,
                           Rect_f32 rect, CalcInterpretGraph *first_graph,
                           CalcInterpretContext *context)
{
    CalcNode *parent_call = first_graph->parent_call;
    Rect_f32 plot_view = first_graph->plot_view;
    
    int plot_count = 0;
    for(CalcInterpretGraph *graph = first_graph; graph && graph->parent_call == parent_call;
        graph = graph->next)
    {
        ++plot_count;
    }
    
    PlotData2D plot_data = {0};
    {
        plot_data.mode           = first_graph->mode;
        plot_data.title          = { first_graph->plot_title, (u64)first_graph->plot_title_length };
        plot_data.x_axis         = { first_graph->x_axis, (u64)first_graph->x_axis_length };
        plot_data.y_axis         = { first_graph->y_axis, (u64)first_graph->y_axis_length };
        plot_data.screen_rect    = rect;
        plot_data.app            = app;
        plot_data.face_id        = face_id;
        plot_data.plot_view      = plot_view;
        plot_data.num_bins       = first_graph->num_bins;
        plot_data.bin_data_range = first_graph->bin_data_range;
        
        if(first_graph->num_bins > 0)
        {
            plot_data.bin_group_count = plot_count;
            plot_data.bins = (int *)MemoryArenaAllocate(context->arena, sizeof(*plot_data.bins)*plot_data.num_bins*
                                                        plot_data.bin_group_count);
            MemorySet(plot_data.bins, 0, sizeof(*plot_data.bins)*plot_data.num_bins*plot_data.bin_group_count);
        }
    }
    Fleury4BeginPlot2D(&plot_data);
    
    for(CalcInterpretGraph *graph = first_graph; graph && graph->parent_call == parent_call;
        graph = graph->next)
    {
        CalcNode *expression = graph->graph_expression;
        CalcInterpretResult expression_result = Fleury4InterpretCalcExpression(context, expression);
        
        switch(plot_data.mode)
        {
            
            
            //~ NOTE(rjf): Line Graphs
            case PLOT2D_MODE_LINE:
            {
                
                // NOTE(rjf): Graphing scripting arrays.
                if(expression_result.value.type == CALC_TYPE_array)
                {
                    
                    // NOTE(rjf): Just Y data.
                    if(expression_result.value.array_count > 0 &&
                       expression_result.value.as_array[0].type == CALC_TYPE_number)
                    {
                        int values_to_plot = expression_result.value.array_count;
                        float *x_values = (float *)MemoryArenaAllocate(context->arena, sizeof(float)*values_to_plot);
                        float *y_values = (float *)MemoryArenaAllocate(context->arena, sizeof(float)*values_to_plot);
                        
                        for(int i = 0; i < values_to_plot; ++i)
                        {
                            x_values[i] = (float)i;
                            y_values[i] = (float)expression_result.value.as_array[i].as_f64;
                        }
                        
                        Fleury4Plot2DPoints(&plot_data, PLOT2D_POINTS, x_values, y_values, values_to_plot);
                    }
                    
                    // NOTE(rjf): X/Y data arrays.
                    else if(expression_result.value.array_count == 2 &&
                            expression_result.value.as_array[0].type == CALC_TYPE_array &&
                            expression_result.value.as_array[1].type == CALC_TYPE_array &&
                            expression_result.value.as_array[0].array_count > 0 &&
                            expression_result.value.as_array[0].array_count ==
                            expression_result.value.as_array[1].array_count &&
                            expression_result.value.as_array[0].as_array[0].type ==
                            expression_result.value.as_array[1].as_array[0].type &&
                            expression_result.value.as_array[0].as_array[0].type == CALC_TYPE_number)
                    {
                        int values_to_plot = expression_result.value.as_array[0].array_count;
                        float *x_values = (float *)MemoryArenaAllocate(context->arena, sizeof(float)*values_to_plot);
                        float *y_values = (float *)MemoryArenaAllocate(context->arena, sizeof(float)*values_to_plot);
                        
                        for(int i = 0; i < values_to_plot; ++i)
                        {
                            x_values[i] = (float)expression_result.value.as_array[0].as_array[i].as_f64;
                            y_values[i] = (float)expression_result.value.as_array[1].as_array[i].as_f64;
                        }
                        
                        Fleury4Plot2DPoints(&plot_data, PLOT2D_POINTS, x_values, y_values, values_to_plot);
                    }
                    
                }
                
                // NOTE(rjf): Graphing data from source code.
                else if(expression_result.value.type == CALC_TYPE_source_code_reference)
                {
                    float *data = 0;
                    int data_count = 0;
                    Fleury4GetDataFromSourceCode(context->app, context->buffer, context->text_layout_id,
                                                 expression_result.value.as_token_offset, context->arena,
                                                 &data, &data_count);
                    
                    // NOTE(rjf): Plot data.
                    if(data && data_count)
                    {
                        float *x_data = (float *)MemoryArenaAllocate(context->arena, data_count * sizeof(*x_data));
                        for(int i = 0; i < data_count; ++i)
                        {
                            x_data[i] = (float)i;
                        }
                        Fleury4Plot2DPoints(&plot_data, PLOT2D_POINTS, x_data, data, data_count);
                    }
                }
                
                // NOTE(rjf): Graphing scripting functions.
                else
                {
                    CalcNode *value = graph->input_value;
                    CalcNode value_node = {0};
                    {
                        value_node.type = CALC_NODE_TYPE_number;
                    }
                    
                    if(value)
                    {
                        CalcSymbolTableAdd(context->symbol_table, value->token.string,
                                           value->token.string_length, &value_node);
                    }
                    
                    // NOTE(rjf): Find function sample points.
                    int values_to_plot = graph->num_function_samples;
                    float *x_values = (float *)MemoryArenaAllocate(context->arena, values_to_plot * sizeof(*x_values));
                    float *y_values = (float *)MemoryArenaAllocate(context->arena, values_to_plot * sizeof(*y_values));
                    {
                        for(int i = 0; i < values_to_plot; ++i)
                        {
                            value_node.value = plot_view.x0 + (i / (float)values_to_plot) * (plot_view.x1 - plot_view.x0);
                            CalcInterpretResult result = Fleury4InterpretCalcExpression(context, expression);
                            if(result.value.type != CALC_TYPE_number)
                            {
                                goto end_graph;
                            }
                            else
                            {
                                x_values[i] = (float)value_node.value;
                                y_values[i] = (float)result.value.as_f64;
                            }
                        }
                    }
                    
                    Fleury4Plot2DPoints(&plot_data, PLOT2D_LINES, x_values, y_values, values_to_plot);
                }
                
                break;
            }
            
            
            //~ NOTE(rjf): Histogram
            case PLOT2D_MODE_HISTOGRAM:
            {
                
                // NOTE(rjf): Graphing scripting arrays.
                if(expression_result.value.type == CALC_TYPE_array)
                {
                    
                    if(expression_result.value.array_count > 0 &&
                       expression_result.value.as_array[0].type == CALC_TYPE_number)
                    {
                        int values_to_plot = expression_result.value.array_count;
                        float *values = (float *)MemoryArenaAllocate(context->arena, sizeof(float)*values_to_plot);
                        
                        for(int i = 0; i < values_to_plot; ++i)
                        {
                            values[i] = (float)expression_result.value.as_array[i].as_f64;
                        }
                        
                        Fleury4Plot2DHistogram(&plot_data, values, values_to_plot);
                    }
                    
                }
                
                // NOTE(rjf): Graphing data from source code.
                else if(expression_result.value.type == CALC_TYPE_source_code_reference)
                {
                    float *data = 0;
                    int data_count = 0;
                    Fleury4GetDataFromSourceCode(context->app, context->buffer, context->text_layout_id,
                                                 expression_result.value.as_token_offset, context->arena,
                                                 &data, &data_count);
                    
                    // NOTE(rjf): Plot data.
                    if(data && data_count)
                    {
                        Fleury4Plot2DHistogram(&plot_data, data, data_count);
                    }
                }
                
                // NOTE(rjf): Graphing scripting functions.
                else
                {
                    
                }
                
                break;
            }
            
            default: break;
        }
        
        end_graph:;
    }
    
    Fleury4EndPlot2D(&plot_data);
}

typedef struct CalcFindInputResult CalcFindInputResult;
struct CalcFindInputResult
{
    CalcNode *unknown;
    int number_unknowns;
};

static CalcFindInputResult
Fleury4FindUnknownForGraph(CalcSymbolTable *table, CalcNode *expression)
{
    CalcFindInputResult result = {0};
    
    if(expression && expression->type != CALC_NODE_TYPE_invalid)
    {
        if(expression->type == CALC_NODE_TYPE_identifier)
        {
            CalcSymbolValue *symbol_value =
                CalcSymbolTableLookup_(table, expression->token.string,
                                       expression->token.string_length);
            
            if(!symbol_value)
            {
                result.unknown = expression;
                ++result.number_unknowns;
            }
        }
        else
        {
            CalcFindInputResult results[] =
            {
                Fleury4FindUnknownForGraph(table, expression->left),
                Fleury4FindUnknownForGraph(table, expression->right),
                Fleury4FindUnknownForGraph(table, expression->first_parameter),
                Fleury4FindUnknownForGraph(table, expression->next),
            };
            
            for(int i = 0; i < ArrayCount(results); ++i)
            {
                if(results[i].unknown)
                {
                    if(!result.unknown)
                    {
                        result.unknown = results[i].unknown;
                        ++result.number_unknowns;
                    }
                    else
                    {
                        if(!StringMatchCaseSensitive(results[i].unknown->token.string, results[i].unknown->token.string_length,
                                                     result.unknown->token.string, result.unknown->token.string_length))
                        {
                            ++result.number_unknowns;
                        }
                    }
                }
            }
        }
    }
    
    return result;
}

#define CALC_BUILT_IN_FUNCTION(name) CalcInterpretResult name(CalcInterpretContext *context, int param_count, CalcInterpretResult *params)
typedef CALC_BUILT_IN_FUNCTION(CalcBuiltInFunction);

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcSin)
{
    CalcInterpretResult result = {0};
    result.value = CalcValueF64(sin(params[0].value.as_f64));
    return result;
}

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcCos)
{
    CalcInterpretResult result = {0};
    result.value = CalcValueF64(cos(params[0].value.as_f64));
    return result;
}

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcTan)
{
    CalcInterpretResult result = {0};
    result.value = CalcValueF64(tan(params[0].value.as_f64));
    return result;
}

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcAbs)
{
    CalcInterpretResult result = {0};
    result.value = CalcValueF64(fabs(params[0].value.as_f64));
    return result;
}

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcPlotTitle)
{
    context->plot_title = params[0].value.as_string + 1;
    context->plot_title_length = params[0].value.string_length - 2;
    CalcInterpretResult result = {0};
    result.value = CalcValueNone();
    return result;
}

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcPlotFunctionSamples)
{
    context->num_function_samples = (int)params[0].value.as_f64;
    CalcInterpretResult result = {0};
    result.value = CalcValueNone();
    return result;
}

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcPlotBinCount)
{
    context->num_bins = (int)params[0].value.as_f64;
    CalcInterpretResult result = {0};
    result.value = CalcValueNone();
    return result;
}

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcPlotBinRange)
{
    context->bin_range_low = (f32)params[0].value.as_f64;
    context->bin_range_high = (f32)params[1].value.as_f64;
    CalcInterpretResult result = {0};
    result.value = CalcValueNone();
    return result;
}

static
CALC_BUILT_IN_FUNCTION(Fleury4CalcTime)
{
    CalcInterpretResult result = {0};
    result.value = CalcValueF64((f64)context->current_time);
    animate_in_n_milliseconds(context->app, 0);
    return result;
}

static CalcInterpretResult
Fleury4CallCalcBuiltInFunction(CalcInterpretContext *context, CalcNode *root)
{
    
#define MAX_BUILTIN_PARAM 4
    
    CalcInterpretResult result = {0};
    
    if(!root || root->type != CALC_NODE_TYPE_function_call)
    {
        result.value = CalcValueError("Internal parsing error, function call expected.");
        goto end_func_call;
    }
    
    static struct
    {
        char *name;
        CalcBuiltInFunction *proc;
        CalcType return_type;
        int required_parameter_count;
        CalcType parameter_types[MAX_BUILTIN_PARAM];
    }
    functions[] =
    {
        { "sin", Fleury4CalcSin, CALC_TYPE_number, 1, { CALC_TYPE_number }, },
        { "cos", Fleury4CalcCos, CALC_TYPE_number, 1, { CALC_TYPE_number }, },
        { "tan", Fleury4CalcTan, CALC_TYPE_number, 1, { CALC_TYPE_number }, },
        { "abs", Fleury4CalcAbs, CALC_TYPE_number, 1, { CALC_TYPE_number }, },
        
        {
            "plot_title",
            Fleury4CalcPlotTitle,
            CALC_TYPE_none,
            1, { CALC_TYPE_string },
        },
        
        {
            "plot_function_samples",
            Fleury4CalcPlotFunctionSamples,
            CALC_TYPE_none,
            1, { CALC_TYPE_number },
        },
        
        {
            "plot_bin_count",
            Fleury4CalcPlotBinCount,
            CALC_TYPE_none,
            1, { CALC_TYPE_number },
        },
        
        {
            "plot_bin_range",
            Fleury4CalcPlotBinRange,
            CALC_TYPE_none,
            2, { CALC_TYPE_number, CALC_TYPE_number },
        },
        
        { "time", Fleury4CalcTime, CALC_TYPE_number, },
        
    };
    
    for(int i = 0; i < ArrayCount(functions); ++i)
    {
        if(Fleury4CalcTokenMatch(root->token, functions[i].name))
        {
            int param_count = 0;
            CalcInterpretResult param_results[MAX_BUILTIN_PARAM] = {0};
            for(CalcNode *param = root->first_parameter; param; param = param->next)
            {
                param_results[param_count++] = Fleury4InterpretCalcExpression(context, param);
                if(param_count >= ArrayCount(param_results))
                {
                    break;
                }
            }
            
            int correct_call = 1;
            
            if(param_count < functions[i].required_parameter_count)
            {
                char *error_string =
                    MakeCStringOnMemoryArena(context->arena, "%s expects at least %i parameters.",
                                             functions[i].name, functions[i].required_parameter_count);
                result.value = CalcValueError(error_string);
                correct_call = 0;
            }
            
            if(correct_call)
            {
                for(int j = 0; j < param_count; ++j)
                {
                    if(param_results[j].value.type != functions[i].parameter_types[j])
                    {
                        correct_call = 0;
                        char *error_string =
                            MakeCStringOnMemoryArena(context->arena, "'%s' expects a '%s' for parameter %i.",
                                                     functions[i].name, Fleury4CalcTypeName(functions[i].parameter_types[j]),
                                                     j+1);
                        result.value = CalcValueError(error_string);
                        break;
                    }
                }
            }
            
            if(correct_call)
            {
                result = functions[i].proc(context, param_count, param_results);
            }
        }
        else if(i == ArrayCount(functions) - 1)
        {
            if(Fleury4CalcTokenMatch(root->token, "plot_xaxis") ||
               Fleury4CalcTokenMatch(root->token, "plot_yaxis"))
            {
                int is_y_axis = Fleury4CalcTokenMatch(root->token, "plot_yaxis");
                
                result.value = CalcValueNone();
                
                CalcNode *title_param = 0;
                CalcNode *low_param = 0;
                CalcNode *high_param = 0;
                
                CalcInterpretResult title_result = {0};
                CalcInterpretResult low_result = {0};
                CalcInterpretResult high_result = {0};
                
                for(CalcNode *param = root->first_parameter;
                    param; param = param->next)
                {
                    CalcInterpretResult interpret =
                        Fleury4InterpretCalcExpression(context, param);
                    
                    if(interpret.value.type == CALC_TYPE_string)
                    {
                        if(title_param)
                        {
                            result.value = CalcValueError(is_y_axis
                                                          ? "plot_yaxis only accepts one string."
                                                          : "plot_xaxis only accepts one string.");
                            goto end_func_call;
                        }
                        else
                        {
                            title_param = param;
                            title_result = interpret;
                        }
                    }
                    else if(interpret.value.type == CALC_TYPE_number)
                    {
                        if(low_param)
                        {
                            if(high_param)
                            {
                                result.value = CalcValueError(is_y_axis
                                                              ? "plot_yaxis only accepts two numbers."
                                                              : "plot_xaxis only accepts two numbers.");
                                
                                goto end_func_call;
                            }
                            else
                            {
                                high_param = param;
                                high_result = interpret;
                            }
                        }
                        else
                        {
                            low_param = param;
                            low_result = interpret;
                        }
                    }
                    else
                    {
                        result = interpret;
                        break;
                    }
                }
                
                if(low_param && high_param)
                {
                    if(is_y_axis)
                    {
                        if(title_result.value.as_string)
                        {
                            context->y_axis = title_result.value.as_string + 1;
                            context->y_axis_length = title_result.value.string_length - 2;
                        }
                        else
                        {
                            context->y_axis = 0;
                            context->y_axis_length = 0;
                        }
                        
                        context->y_low = (f32)low_result.value.as_f64;
                        context->y_high = (f32)high_result.value.as_f64;
                    }
                    else
                    {
                        if(title_result.value.as_string)
                        {
                            context->x_axis = title_result.value.as_string + 1;
                            context->x_axis_length = title_result.value.string_length - 2;
                        }
                        else
                        {
                            context->x_axis = 0;
                            context->x_axis_length = 0;
                        }
                        
                        context->x_low = (f32)low_result.value.as_f64;
                        context->x_high = (f32)high_result.value.as_f64;
                    }
                }
                else
                {
                    result.value = CalcValueError(is_y_axis
                                                  ? "plot_yaxis needs two bounds (title optional)."
                                                  : "plot_xaxis needs two bounds (title optional).");
                }
            }
            
            else if(Fleury4CalcTokenMatch(root->token, "plot") ||
                    Fleury4CalcTokenMatch(root->token, "plot_histogram"))
            {
                
                struct
                {
                    char *name;
                    Plot2DMode mode;
                }
                plot_functions[] =
                {
                    { "plot",           PLOT2D_MODE_LINE,       },
                    { "plot_histogram", PLOT2D_MODE_HISTOGRAM,  },
                };
                
                Plot2DMode mode = PLOT2D_MODE_LINE;
                for(int j = 0; j < ArrayCount(plot_functions); ++j)
                {
                    if(Fleury4CalcTokenMatch(root->token, plot_functions[j].name))
                    {
                        mode = plot_functions[j].mode;
                    }
                }
                
                result.value = CalcValueNone();
                
                CalcInterpretGraph **target = &result.first_graph;
                for(CalcNode *graph_expression = root->first_parameter;
                    graph_expression; graph_expression = graph_expression->next)
                {
                    CalcFindInputResult input_find = Fleury4FindUnknownForGraph(context->symbol_table,
                                                                                graph_expression);
                    if(input_find.number_unknowns <= 1)
                    {
                        CalcNode *input_variable = input_find.unknown;
                        CalcInterpretGraph *new_graph =
                            (CalcInterpretGraph *)MemoryArenaAllocate(context->arena, sizeof(*new_graph));
                        new_graph->graph_expression = graph_expression;
                        new_graph->input_value = input_variable;
                        new_graph->next = 0;
                        new_graph->parent_call = root;
                        new_graph->plot_title = context->plot_title;
                        new_graph->plot_title_length = context->plot_title_length;
                        new_graph->x_axis = context->x_axis;
                        new_graph->x_axis_length = context->x_axis_length;
                        new_graph->y_axis = context->y_axis;
                        new_graph->y_axis_length = context->y_axis_length;
                        new_graph->mode = mode;
                        new_graph->plot_view =
                            Rf32(context->x_low, context->y_low,
                                 context->x_high, context->y_high);
                        new_graph->num_function_samples = context->num_function_samples;
                        new_graph->num_bins = context->num_bins;
                        new_graph->bin_data_range.min = context->bin_range_low;
                        new_graph->bin_data_range.max = context->bin_range_high;
                        *target = new_graph;
                        target = &(*target)->next;
                    }
                    else
                    {
                        result.value = CalcValueError("Too many unknowns in graphing expression.");
                        break;
                    }
                }
            }
            
        }
    }
    
    end_func_call:;
    return result;
}

static CalcInterpretResult
Fleury4InterpretCalcExpression(CalcInterpretContext *context, CalcNode *root)
{
    CalcInterpretResult result = {0};
    
    if(root == 0)
    {
        result.value = CalcValueError("something went wrong");
    }
    else
    {
        switch(root->type)
        {
            case CALC_NODE_TYPE_number:
            {
                result.value = CalcValueF64(root->value);
                break;
            }
            
            case CALC_NODE_TYPE_array:
            {
                result.value = CalcValueArray(context, root->first_member);
                break;
            }
            
            case CALC_NODE_TYPE_string_constant:
            {
                result.value = CalcValueString(root->token.string, root->token.string_length);
                break;
            }
            
            case CALC_NODE_TYPE_add:
            case CALC_NODE_TYPE_subtract:
            case CALC_NODE_TYPE_multiply:
            case CALC_NODE_TYPE_divide:
            case CALC_NODE_TYPE_raise_to_power:
            {
                CalcInterpretResult left_result = Fleury4InterpretCalcExpression(context, root->left);
                CalcInterpretResult right_result = Fleury4InterpretCalcExpression(context, root->right);
                if(left_result.value.type == CALC_TYPE_error ||
                   right_result.value.type == CALC_TYPE_error)
                {
                    result.value = CalcValueError(left_result.value.type == CALC_TYPE_error ? left_result.value.as_error : right_result.value.as_error);
                    goto end_interpret;
                }
                
                else if(left_result.value.type != CALC_TYPE_number ||
                        right_result.value.type != CALC_TYPE_number)
                {
                    result.value = CalcValueError("Cannot use non-numbers in expressions.");
                    goto end_interpret;
                }
                
                switch(root->type)
                {
                    case CALC_NODE_TYPE_add:            result.value = CalcValueF64(left_result.value.as_f64 + right_result.value.as_f64); break;
                    case CALC_NODE_TYPE_subtract:       result.value = CalcValueF64(left_result.value.as_f64 - right_result.value.as_f64); break;
                    case CALC_NODE_TYPE_multiply:       result.value = CalcValueF64(left_result.value.as_f64 * right_result.value.as_f64); break;
                    case CALC_NODE_TYPE_divide:
                    {
                        if(right_result.value.as_f64 == 0)
                        {
                            result.value = CalcValueF64(NAN);
                        }
                        else
                        {
                            result.value = CalcValueF64(left_result.value.as_f64 / right_result.value.as_f64);
                        }
                        break;
                    }
                    case CALC_NODE_TYPE_raise_to_power:
                    {
                        result.value = CalcValueF64(pow(left_result.value.as_f64, right_result.value.as_f64));
                        break;
                    }
                }
                
                break;
            }
            
            case CALC_NODE_TYPE_negate:
            {
                result = Fleury4InterpretCalcExpression(context, root->operand);
                if(result.value.type == CALC_TYPE_number)
                {
                    result.value = CalcValueF64(-result.value.as_f64);
                }
                break;
            }
            
            case CALC_NODE_TYPE_function_call:
            {
                result = Fleury4CallCalcBuiltInFunction(context, root);
                break;
            }
            
            case CALC_NODE_TYPE_identifier:
            {
                if(Fleury4CalcTokenMatch(root->token, "e"))
                {
                    result.value = CalcValueF64(2.71828);
                }
                else if(Fleury4CalcTokenMatch(root->token, "pi"))
                {
                    result.value = CalcValueF64(3.1415926535897);
                }
                else
                {
                    CalcNode *value = CalcSymbolTableLookup(context->symbol_table, root->token.string, root->token.string_length);
                    result = Fleury4InterpretCalcExpression(context, value);
                }
                
                break;
            }
            
            case CALC_NODE_TYPE_source_code_identifier:
            {
                Token_Array token_array = get_token_array_from_buffer(context->app, context->buffer);
                Range_i64 visible_range = text_layout_get_visible_range(context->app, context->text_layout_id);
                i64 first_index = token_index_from_pos(&token_array, visible_range.first);
                Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
                Token *token = 0;
                
                for(;;)
                {
                    token = token_it_read(&it);
                    
                    if(token->pos >= visible_range.one_past_last + 4096 || !token || !token_it_inc_non_whitespace(&it))
                    {
                        break;
                    }
                    
                    if(token->kind == TokenBaseKind_Identifier)
                    {
                        String_Const_u8 token_string;
                        {
                            Range_i64 token_range =
                            {
                                token->pos,
                                token->pos + (token->size > (i64)256
                                              ? (i64)256
                                              : token->size),
                            };
                            
                            u8 token_buffer[256] = {0};
                            buffer_read_range(context->app, context->buffer, token_range, token_buffer);
                            token_string = { token_buffer, (u64)(token_range.end - token_range.start) };
                        }
                        
                        if(StringMatchCaseSensitive((char *)token_string.data, (int)token_string.size,
                                                    root->token.string, root->token.string_length))
                        {
                            result.value = CalcValueSourceCodeReference(token->pos);
                            break;
                        }
                    }
                }
                
                break;
            }
            
            default: break;
        }
    }
    
    end_interpret:;
    return result;
}

static int
Fleury4IdentifierExistsInCalcExpression(CalcNode *root, char *string, int string_length)
{
    int result = 0;
    
    if(root && root->type != CALC_NODE_TYPE_invalid)
    {
        if(StringMatchCaseSensitive(root->token.string, root->token.string_length, string, string_length))
        {
            result = 1;
        }
        else
        {
            result |= Fleury4IdentifierExistsInCalcExpression(root->left, string, string_length);
            result |= Fleury4IdentifierExistsInCalcExpression(root->right, string, string_length);
            result |= Fleury4IdentifierExistsInCalcExpression(root->first_parameter, string, string_length);
            result |= Fleury4IdentifierExistsInCalcExpression(root->next, string, string_length);
        }
    }
    
    return result;
}

static CalcInterpretResult
Fleury4InterpretCalcCode(CalcInterpretContext *context, CalcNode *tree_root)
{
    CalcInterpretResult result = {0};
    CalcInterpretResult last_result = result;
    
    for(CalcNode *root = tree_root; root; root = root->next)
    {
        last_result = result;
        
        if(root->type == CALC_NODE_TYPE_assignment)
        {
            if(root->left->type == CALC_NODE_TYPE_identifier)
            {
                if(!Fleury4IdentifierExistsInCalcExpression(root->right, root->left->token.string, root->left->token.string_length))
                {
                    CalcSymbolTableAdd(context->symbol_table, root->left->token.string,
                                       root->left->token.string_length, root->right);
                    result = Fleury4InterpretCalcExpression(context, root->right);
                    result.first_graph = last_result.first_graph;
                }
                else
                {
                    result.value = CalcValueError("Recursive definition.");
                    result.first_graph = last_result.first_graph;
                }
            }
            else
            {
                result.value = CalcValueError("Assignment to non-identifier.");
                result.first_graph = last_result.first_graph;
            }
        }
        else
        {
            result = Fleury4InterpretCalcExpression(context, root);
            if(last_result.first_graph)
            {
                for(CalcInterpretGraph *graph = last_result.first_graph; graph; graph = graph->next)
                {
                    if(graph->next == 0)
                    {
                        graph->next = result.first_graph;
                        break;
                    }
                }
                
                result.first_graph = last_result.first_graph;
            }
            else if(result.value.type == CALC_TYPE_error)
            {
                break;
            }
        }
    }
    
    return result;
}

static void
Fleury4RenderCalcComments(Application_Links *app, Buffer_ID buffer, View_ID view,
                          Text_Layout_ID text_layout_id, Frame_Info frame_info)
{
    static f32 current_time = 0;
    current_time += frame_info.literal_dt;
    
    Token_Array token_array = get_token_array_from_buffer(app, buffer);
    Range_i64 visible_range = text_layout_get_visible_range(app, text_layout_id);
    Scratch_Block scratch(app);
    
    if(token_array.tokens != 0)
    {
        static char arena_buffer[64*1024*1024];
        MemoryArena arena = MemoryArenaInit(arena_buffer, sizeof(arena_buffer));
        CalcSymbolTable symbol_table = CalcSymbolTableInit(&arena, 1024);
        
        CalcInterpretContext context_ = {0};
        CalcInterpretContext *context = &context_;
        context->app = app;
        context->buffer = buffer;
        context->text_layout_id = text_layout_id;
        context->arena = &arena;
        context->symbol_table = &symbol_table;
        context->num_function_samples = 128;
        context->current_time = current_time;
        
        i64 first_index = token_index_from_pos(&token_array, visible_range.first);
        Token_Iterator_Array it = token_iterator_index(0, &token_array, first_index);
        
        Token *token = 0;
        for(;;)
        {
            token = token_it_read(&it);
            
            if(token->pos >= visible_range.one_past_last || !token || !token_it_inc_non_whitespace(&it))
            {
                break;
            }
            
            if(token->kind == TokenBaseKind_Comment)
            {
                Rect_f32 comment_first_char_rect =
                    text_layout_character_on_screen(app, text_layout_id, token->pos);
                
                Rect_f32 comment_last_char_rect =
                    text_layout_character_on_screen(app, text_layout_id, token->pos + token->size - 1);
                
                Range_i64 token_range =
                {
                    token->pos,
                    token->pos + (token->size > 1024
                                  ? 1024
                                  : token->size),
                };
                
                u32 token_buffer_size = (u32)(token_range.end - token_range.start);
                if(token_buffer_size < 4)
                {
                    token_buffer_size = 4;
                }
                u8 *token_buffer = (u8 *)MemoryArenaAllocate(&arena, token_buffer_size+1);
                buffer_read_range(app, buffer, token_range, token_buffer);
                token_buffer[token_buffer_size] = 0;
                
                if((token_buffer[0] == '/' && token_buffer[1] == '/' && token_buffer[2] == 'c' &&
                    token_buffer[3] <= 32) ||
                   (token_buffer[0] == '/' && token_buffer[1] == '*' && token_buffer[2] == 'c'))
                    
                {
                    int is_multiline_comment = (token_buffer[1] == '*');
                    if(is_multiline_comment)
                    {
                        if(token_buffer[token_buffer_size-1] == '/' &&
                           token_buffer[token_buffer_size-2] == '*')
                        {
                            token_buffer[token_buffer_size-2] = 0;
                        }
                    }
                    
                    char *at = (char *)token_buffer + 3;
                    CalcNode *expr = Fleury4ParseCalcCode(&arena, &at);
                    CalcInterpretResult result = Fleury4InterpretCalcCode(context, expr);
                    
                    char result_buffer[256] = {0};
                    String_Const_u8 result_string =
                    {
                        (u8 *)result_buffer,
                    };
                    
                    switch(result.value.type)
                    {
                        case CALC_TYPE_error:
                        {
                            if(expr == 0)
                            {
                                result_string.size = (u64)snprintf(result_buffer, sizeof(result_buffer),
                                                                   "= (syntax error: \'parse failed\')");
                            }
                            else
                            {
                                result_string.size = (u64)snprintf(result_buffer, sizeof(result_buffer),
                                                                   "= (syntax error: \'%s\')", result.value.as_error);
                            }
                            break;
                        }
                        case CALC_TYPE_number:
                        {
                            result_string.size = (u64)snprintf(result_buffer, sizeof(result_buffer),
                                                               "= %f", result.value.as_f64);
                            break;
                        }
                        case CALC_TYPE_string:
                        {
                            result_string.size = (u64)snprintf(result_buffer, sizeof(result_buffer),
                                                               "= '%.*s'", result.value.string_length, result.value.as_string);
                            break;
                        }
                        default: break;
                    }
                    
                    Vec2_f32 point =
                    {
                        comment_last_char_rect.x1 + 20,
                        comment_first_char_rect.y0,
                    };
                    
                    u32 color = finalize_color(defcolor_comment, 0);
                    color &= 0x00ffffff;
                    color |= 0x80000000;
                    draw_string(app, get_face_id(app, buffer), result_string, point, color);
                    
                    Rect_f32 view_rect = view_get_screen_rect(app, view);
                    
                    Rect_f32 graph_rect = {0};
                    {
                        graph_rect.x0 = view_rect.x1 - 30 - 300;
                        graph_rect.y0 = comment_first_char_rect.y0;
                        graph_rect.x1 = graph_rect.x0 + 300;
                        graph_rect.y1 = graph_rect.y0 + 200;
                    }
                    
                    CalcNode *last_parent_call = 0;
                    for(CalcInterpretGraph *graph = result.first_graph; graph;
                        graph = graph->next)
                    {
                        if(last_parent_call == 0 || graph->parent_call != last_parent_call)
                        {
                            Fleury4GraphCalcExpression(app, get_face_id(app, buffer), graph_rect, graph, context);
                            
                            // NOTE(rjf): Bump graph rect forward.
                            {
                                f32 rect_height = graph_rect.y1 - graph_rect.y0;
                                graph_rect.y0 += rect_height + 50;
                                graph_rect.y1 += rect_height + 50;
                            }
                            
                            last_parent_call = graph->parent_call;
                        }
                    }
                    
                }
                
            }
            
        }
        
    }
    
}
