import sys
from pycparser import parse_file

if __name__ == "__main__":
    filename = 'include/fmod_studio.h'

    ast = parse_file(filename, use_cpp=True,
            cpp_path='gcc',
            cpp_args=['-E'])
    ast.show()
