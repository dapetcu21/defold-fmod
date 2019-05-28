import sys
import re
from pycparser import parse_file, c_ast
from jinja2 import Environment, FileSystemLoader, select_autoescape

TypeEnum = 1
TypeStruct = 2
TypeOpaqueStruct = 3

def generate_bindings(ast):
    types = {}
    enums = []
    structs = {}

    def parse_struct(struct):
        if struct.decls == None:
            if struct.name in types and types[struct.name] != TypeStruct:
                types[struct.name] = TypeOpaqueStruct
        else:
            types[struct.name] = TypeStruct
            structs[struct.name] = struct

    for node in ast:
        if isinstance(node, c_ast.Typedef):
            if isinstance(node.type, c_ast.TypeDecl):
                if isinstance(node.type.type, c_ast.Enum):
                    types[node.name] = TypeEnum
                    for enumlist in node.type.type:
                        for enum in enumlist:
                            if re.search("_FORCEINT$", enum.name) == None:
                                enums.append(re.sub("^FMOD_", "", enum.name))

                elif isinstance(node.type.type, c_ast.Struct):
                    parse_struct(node.type.type)

                else:
                    node.show()

        elif isinstance(node, c_ast.Decl):
            if isinstance(node.type, c_ast.Struct):
                parse_struct(node.type)

            elif isinstance(node.type, c_ast.FuncDecl):
                print('FuncDecl: %s' % (node.name))

            else:
                node.show()

        else:
            node.show()

    print('Types:', types)
    print('Structs:', structs.keys())

    env = Environment(
        loader = FileSystemLoader('.'),
        autoescape = False,
    )
    template = env.get_template('fmod_generated_template.cpp')

    output = template.render(enums=enums)

    with open('src/fmod_generated.cpp', 'w') as f:
        f.write(output)


if __name__ == "__main__":
    filename = 'include/fmod_studio.h'

    ast = parse_file(filename, use_cpp=True,
            cpp_path='gcc',
            cpp_args=['-E'])

    generate_bindings(ast)
    # ast.show()
