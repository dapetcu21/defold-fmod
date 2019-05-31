import sys
import re
from pycparser import parse_file, c_ast
from jinja2 import Environment, FileSystemLoader, select_autoescape

TypeBasic = 1
TypeStruct = 2
TypeOpaqueStruct = 3
TypePointer = 4
TypeUnknown = 5

def generate_bindings(ast):
    types = {}
    enums = []
    structs = {}
    basic_types = {}

    class ParsedTypeDecl:
        def __init__(self, *, node=None, name=None, c_type="", type=TypeBasic, readable=True, writeable=True):
            if node != None:
                if isinstance(node, c_ast.PtrDecl):
                    child = ParsedTypeDecl(node=node.type)
                    self.name = "ptr_" + child.name
                    self.c_type = child.c_type + "*"
                    self.type = TypePointer

                    base_type = types[child.name] if child.name in types else TypeUnknown
                    self.readable = base_type == TypeStruct or base_type == TypeOpaqueStruct or child.c_type == "char"
                    self.writeable = False

                    self.child = child
                    return

                if isinstance(node, c_ast.TypeDecl) and isinstance(node.type, c_ast.IdentifierType):
                    name = "_".join(node.type.names)
                    c_type = " ".join(node.type.names)

                    if name in basic_types:
                        other = basic_types[name]
                        self.name = other.name
                        self.c_type = other.c_type
                        self.readable = other.readable
                        self.writeable = other.writeable
                        self.type = other.type
                        return

                    self.name = name
                    self.c_type = c_type
                    self.type = types[name] if name in types else TypeUnknown
                    self.readable = self.type == TypeStruct
                    self.writeable = self.type == TypeStruct
                    return

                self.name = '__UNKNOWN__'
                self.c_type = '__UNKNOWN__'
                self.type = TypeUnknown
                self.readable = False
                self.writable = False

            else:
                self.c_type = c_type
                self.name = name if name != None else re.sub(" ", "_", c_type)
                self.type = type
                self.readable = readable
                self.writeable = writeable

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
                        type_decl = ParsedTypeDecl(node=node.type)
                        properties.append((node.name, type_decl))

            StructVisitor().visit(struct)


    def parse_struct(struct):
        if struct.decls == None:
            if struct.name in types and types[struct.name] != TypeStruct:
                types[struct.name] = TypeOpaqueStruct
        else:
            types[struct.name] = TypeStruct
            structs[struct.name] = ParsedStruct(struct)

    basic_types["char"] = ParsedTypeDecl(c_type="char")
    basic_types["short"] = ParsedTypeDecl(c_type="short")
    basic_types["long"] = ParsedTypeDecl(c_type="long")
    basic_types["int"] = ParsedTypeDecl(c_type="int")
    basic_types["unsigned_char"] = ParsedTypeDecl(c_type="unsigned char")
    basic_types["unsigned_short"] = ParsedTypeDecl(c_type="unsigned short")
    basic_types["unsigned_long"] = ParsedTypeDecl(c_type="unsigned long")
    basic_types["unsigned_int"] = ParsedTypeDecl(c_type="unsigned int")
    basic_types["FMOD_BOOL"] = ParsedTypeDecl(c_type="FMOD_BOOL")
    basic_types["float"] = ParsedTypeDecl(c_type="float")
    basic_types["double"] = ParsedTypeDecl(c_type="double")
    basic_types["ptr_char"] = ParsedTypeDecl(name="ptr_char", c_type="char*", writeable=False, type=TypePointer)

    for key in basic_types.keys():
        types[key] = TypeBasic

    int_type = basic_types["int"]

    for node in ast:
        if isinstance(node, c_ast.Typedef):
            if isinstance(node.type, c_ast.TypeDecl):
                if isinstance(node.type.type, c_ast.Enum):
                    types[node.name] = TypeBasic
                    basic_types[node.name] = int_type
                    for enumlist in node.type.type:
                        for enum in enumlist:
                            if re.search("_FORCEINT$", enum.name) == None:
                                enums.append(re.sub("^FMOD_", "", enum.name))

                elif isinstance(node.type.type, c_ast.Struct):
                    parse_struct(node.type.type)

                elif node.name not in basic_types:
                    parsed_type = ParsedTypeDecl(node=node.type)
                    if parsed_type.name in basic_types:
                        basic_types[node.name] = basic_types[parsed_type.name]
                        types[node.name] = TypeBasic
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
