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

    class ParsedTypeDecl:
        def __init__(self, decl):
            self.name = 'int'
            self.c_type = 'int'
            self.readable = True
            self.writeable = True


    class ParsedStruct:
        def __init__(self, struct):
            self.name = struct.name

            constructor_name = struct.name
            constructor_name = re.sub("^FMOD_STUDIO_", "", constructor_name)
            self.constructor_table = -1
            if constructor_name == struct.name:
                self.constructor_table = -2
                constructor_name = re.sub("^FMOD_", "", constructor_name)
            constructor_name = re.sub("^([0-9])", r"_\1", constructor_name)
            self.constructor_name = constructor_name

            properties = []
            self.properties = properties

            class StructVisitor(c_ast.NodeVisitor):
                def visit_Decl(self, node):
                    if node.name != None:
                        type_decl = ParsedTypeDecl(node.type)
                        properties.append((node.name, type_decl))

            StructVisitor().visit(struct)


    def parse_struct(struct):
        if struct.decls == None:
            if struct.name in types and types[struct.name] != TypeStruct:
                types[struct.name] = TypeOpaqueStruct
        else:
            types[struct.name] = TypeStruct
            structs[struct.name] = ParsedStruct(struct)

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

    output = template.render(
        enums = enums,
        structs = list(structs.values()),
    )

    with open('src/fmod_generated.cpp', 'w') as f:
        f.write(output)


if __name__ == "__main__":
    filename = 'include/fmod_studio.h'

    ast = parse_file(filename, use_cpp=True,
            cpp_path='gcc',
            cpp_args=['-E'])

    generate_bindings(ast)
    # ast.show()
