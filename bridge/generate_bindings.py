import sys
import re
from pycparser import parse_file, c_ast
from jinja2 import Environment, FileSystemLoader, select_autoescape

TypeBasic = 1
TypeStruct = 2
TypeClass = 3
TypePointer = 4
TypeUnknown = 5

exclusions = {}

valid = re.compile(r"^_*(IDs|[A-Z][a-z]+|[A-Z0-9]+(?![a-z]))")
def to_snake_case(s):
    components = []
    while True:
        match = valid.match(s)
        if match == None:
            break
        components.append(match.group(1).lower())
        s = s[match.end():]
    return "_".join(components)

enum_re = re.compile(r"^\s*#define FMOD_([a-zA-Z0-9_]+)")
enum_exceptions = {"STUDIO_COMMON_H": True, "STUDIO_H": True}
def add_defined_enums(enums):
    headers = ["include/fmod_common.h", "include/fmod_studio_common.h"]
    for filename in headers:
        with open(filename, "r") as f:
            line = f.readline()
            while line:
                match = enum_re.match(line)
                if match != None:
                    enum = match.group(1)
                    if not enum.startswith("PRESET_") and not enum in enum_exceptions:
                        enums.append(enum)
                line = f.readline()

def generate_bindings(ast):
    types = {}
    enums = []
    functions = []
    global_functions = []
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
                    self.readable = base_type == TypeStruct or base_type == TypeClass or child.c_type == "char"
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
        def __init__(self):
            self.methods = []
            self.properties = []

        def parse_struct(self, node):
            self.name = node.name
            self.is_class = False

            constructor_name = node.name
            constructor_name = re.sub("^FMOD_STUDIO_", "", constructor_name)
            self.constructor_table = -1
            if constructor_name == node.name:
                self.constructor_table = -2
                constructor_name = re.sub("^FMOD_", "", constructor_name)
            constructor_name = re.sub("^([0-9])", r"_\1", constructor_name)
            self.constructor_name = constructor_name

            properties = self.properties
            class StructVisitor(c_ast.NodeVisitor):
                def visit_Decl(self, node):
                    if node.name != None:
                        type_decl = ParsedTypeDecl(node=node.type)
                        properties.append((node.name, type_decl))

            StructVisitor().visit(node)

        def make_class(self, name):
            self.name = name
            self.is_class = True

    class MethodArgument:
        def __init__(self, node):
            self.name = node.name
            self.type = ParsedTypeDecl(node=node.type)

    class ParsedMethod:
        def __init__(self, node):
            self.node = node
            self.name = node.name
            self.args = []

        def parse_arguments(self):
            for param in self.node.type.args.params:
                self.args.append(MethodArgument(param))

        def detect_scope(self):
            first_arg = self.args[0]
            if first_arg != None:
                caps_name = self.name.upper()
                if first_arg.type.type == TypePointer and first_arg.type.child.type == TypeClass:
                    type_name = first_arg.type.child.name
                    if caps_name.startswith(type_name + "_"):
                        method_name = self.name[len(type_name) + 1:]
                        structs[type_name].methods.append((to_snake_case(method_name), self))
                        return

            method_name = self.name
            table_index = -2
            if caps_name.startswith("FMOD_STUDIO_"):
                table_index = -1
                method_name = self.name[len("FMOD_STUDIO_"):]
            elif caps_name.startswith("FMOD_"):
                method_name = self.name[len("FMOD_"):]
            global_functions.append((table_index, to_snake_case(method_name), self))

        def parse(self):
            self.parse_arguments()
            self.detect_scope()

    def parse_struct(struct):
        if struct.name in exclusions:
            return
        if struct.decls == None:
            if struct.name not in structs:
                types[struct.name] = TypeClass
                parsed_class = ParsedStruct()
                parsed_class.make_class(struct.name)
                structs[struct.name] = parsed_class
        else:
            types[struct.name] = TypeStruct
            parsed_struct = structs[struct.name] if struct.name in structs else ParsedStruct()
            parsed_struct.parse_struct(struct)
            structs[struct.name] = parsed_struct

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
                        print("Unknown typedef")
                        node.show()

        elif isinstance(node, c_ast.Decl):
            if isinstance(node.type, c_ast.Struct):
                parse_struct(node.type)

            elif isinstance(node.type, c_ast.FuncDecl):
                functions.append(ParsedMethod(node))

            else:
                print("Unknown declaration")
                node.show()

        else:
            node.show()

    add_defined_enums(enums)

    for function in functions:
        function.parse()

    env = Environment(
        loader = FileSystemLoader('.'),
        autoescape = False,
    )
    template = env.get_template('fmod_generated_template.cpp')

    output = template.render(
        enums = enums,
        structs = list(structs.values()),
        functions = functions,
        global_functions = global_functions,
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
