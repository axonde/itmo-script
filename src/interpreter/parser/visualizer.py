import json
import graphviz
import sys
import os

def visualize_ast_simple(ast_data, output_filename='ast'):
    dot = graphviz.Digraph(
        comment='AST',
        format='png',
        graph_attr={'rankdir': 'TB', 'dpi': '150'},
        node_attr={'shape': 'box', 'style': 'rounded', 'fontname': 'Helvetica'},
    )
    
    node_counter = 0
    node_map = {}
    
    def add_node(data, parent_id=None, edge_label=''):
        nonlocal node_counter
        
        # Создаем уникальный ID узла
        node_id = f"node{node_counter}"
        node_counter += 1
        
        # Определяем тип узла
        node_type = "Unknown"
        if isinstance(data, list):
            for item in data:
                if isinstance(item, list) and len(item) == 2 and item[0] == "type":
                    node_type = item[1]
                    break
        elif isinstance(data, dict) and "type" in data:
            node_type = data["type"]
        
        # Собираем свойства узла
        properties = []
        if isinstance(data, list):
            for item in data:
                if (isinstance(item, list) and 
                    len(item) == 2 and 
                    item[0] != "type" and 
                    not isinstance(item[1], (list, dict))):
                    properties.append(f"{item[0]}: {item[1]}")
        elif isinstance(data, dict):
            for key, value in data.items():
                if key != "type" and not isinstance(value, (list, dict)):
                    properties.append(f"{key}: {value}")
        
        # Формируем подпись узла
        label = node_type
        if properties:
            label += "\n" + "\n".join(properties)
        
        # Добавляем узел
        dot.node(node_id, label)
        node_map[id(data)] = node_id
        
        # Связь с родителем
        if parent_id:
            dot.edge(parent_id, node_id, label=edge_label)
        
        # Обрабатываем детей
        if isinstance(data, list):
            for item in data:
                if (isinstance(item, list) and 
                    len(item) == 2 and 
                    isinstance(item[1], (list, dict))):
                    
                    # Обработка обычных детей
                    if item[0] == "children":
                        if isinstance(item[1], list):
                            for i, child in enumerate(item[1]):
                                add_node(child, node_id, f"child[{i}]")
                    
                    # Обработка специальных полей
                    elif item[0] in ["left", "right", "operand", "condition", "body", 
                                    "iterator", "range", "target", "func"]:
                        add_node(item[1], node_id, item[0])
        
        elif isinstance(data, dict):
            for key, value in data.items():
                if key != "type" and isinstance(value, (list, dict)):
                    
                    # Обработка children
                    if key == "children" and isinstance(value, list):
                        for i, child in enumerate(value):
                            add_node(child, node_id, f"child[{i}]")
                    
                    # Обработка специальных полей
                    elif key in ["left", "right", "operand", "condition", "body", 
                                "iterator", "range", "target", "func"]:
                        add_node(value, node_id, key)
        
        return node_id
    
    # Визуализируем корень
    add_node(ast_data)
    
    # Сохраняем результат
    output_path = dot.render(
        filename=output_filename,
        cleanup=True,
        directory=os.path.dirname(output_filename) or '.'
    )
    
    print(f"AST визуализирован в {output_path}")
    return output_path

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Использование: python ast_visualizer.py <ast.json> [output.png]")
        print("Пример: python ast_visualizer.py ast.json result.png")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else 'ast.png'
    
    try:
        with open(input_file, 'r') as f:
            raw_data = json.load(f)
        
        visualize_ast_simple(raw_data, output_file)
    
    except Exception as e:
        print(f"Ошибка при визуализации AST: {str(e)}")
        sys.exit(1)