import gdb
import gdb.printing

class Vec2Printer:
    """Pretty printer for vec2t<T>"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        # Extract the x, y, z values from the struct
        x = self.val["x"]
        y = self.val["y"]
        return f"[{x}, {y}]"

    def children(self):
        return [("x  r", self.val["x"]), ("y  g", self.val["y"]), ("cell", self.val["cell"])]

class Vec3Printer:
    """Pretty printer for vec3t<T>"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        # Extract the x, y, z values from the struct
        x = self.val["x"]
        y = self.val["y"]
        z = self.val["z"]
        return f"[{x}, {y}, {z}]"

    def children(self):
        return [("x  r  u", self.val["x"]), ("y  g  v", self.val["y"]), ("z  b  w", self.val["z"]), ("cell", self.val["cell"])]

class Vec4Printer:
    """Pretty printer for vec4t<T>"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        # Extract the x, y, z values from the struct
        x = self.val["x"]
        y = self.val["y"]
        z = self.val["z"]
        w = self.val["w"]
        return f"[{x}, {y}, {z}, {w}]"

    def using_simd(self):
        try:
            output = gdb.execute("info macros", to_string=True)
            return "ENGINE_USE_SSE" in output
        except gdb.error:
            return False

    def children(self):
        child_list = [("x  r", self.val["x"]), ("y  g", self.val["y"]), ("z  b", self.val["z"]), ("w  a", self.val["w"]), ("cell", self.val["cell"])]
        try:
            child_list += [("v4", self.val["v4"])]
        except:
            pass
        return child_list

class Mat2Printer:
    """Pretty printer for mat2t<T>"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        # Extract the x, y, z values from the struct
        row0 = self.val["rows"][0]
        row1 = self.val["rows"][1]
        return f"[[{row0['x']}, {row0['y']}], [{row1['x']}, {row1['y']}]]"

    def children(self):
        child_list = [("rows[0]", self.val["rows"][0]), ("rows[1]", self.val["rows"][1]), ("cell", self.val["cell"])]
        return child_list

class Mat3Printer:
    """Pretty printer for mat3t<T>"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        # Extract the x, y, z values from the struct
        row0 = self.val["rows"][0]
        row1 = self.val["rows"][1]
        row2 = self.val["rows"][2]
        return f"[[{row0['x']}, {row0['y']}, {row0['z']}], [{row1['x']}, {row1['y']}, {row1['z']}], [{row2['x']}, {row2['y']}, {row2['z']}]]"

    def children(self):
        child_list = [("rows[0]", self.val["rows"][0]), ("rows[1]", self.val["rows"][1]), ("rows[2]", self.val["rows"][2]), ("cell", self.val["cell"])]
        return child_list

class Mat4Printer:
    """Pretty printer for mat4t<T>"""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        # Extract the x, y, z values from the struct
        row0 = self.val["rows"][0]
        row1 = self.val["rows"][1]
        row2 = self.val["rows"][2]
        row3 = self.val["rows"][3]
        return (f"[[{row0['x']}, {row0['y']}, {row0['z']}, {row0['w']}],"
                f" [{row1['x']}, {row1['y']}, {row1['z']}, {row1['w']}],"
                f" [{row2['x']}, {row2['y']}, {row2['z']}, {row2['w']}],"
                f" [{row3['x']}, {row3['y']}, {row3['z']}, {row3['w']}]]")

    def children(self):
        child_list = [("rows[0]", self.val["rows"][0]), ("rows[1]", self.val["rows"][1]), ("rows[2]", self.val["rows"][2]),
                      ("rows[3]", self.val["rows"][3]), ("cell", self.val["cell"])]
        # SSE
        try:
            child_list += [("row14", self.val["row14"]), ("row24", self.val["row24"]), ("row34", self.val["row34"]), ("row44", self.val["row44"])]
        except:
            pass

        # AVX2
        try:
            child_list += [("rows12", self.val["rows12"]), ("rows34", self.val["rows34"])]
        except:
            pass
        return child_list

def build_pretty_printer(regx, typename, filt, printer):
    pp = gdb.printing.RegexpCollectionPrettyPrinter(regx)
    pp.add_printer(typename, filt, printer)
    return pp

# Add the pretty printer to GDB
gdb.printing.register_pretty_printer(gdb.current_objfile(), build_pretty_printer("Engine::vec2", "vec2t", r"vec2t<.*>", Vec2Printer))
gdb.printing.register_pretty_printer(gdb.current_objfile(), build_pretty_printer("Engine::vec3", "vec3t", r"vec3t<.*>", Vec3Printer))
gdb.printing.register_pretty_printer(gdb.current_objfile(), build_pretty_printer("Engine::vec4", "vec4t", r"vec4t<.*>", Vec4Printer))
gdb.printing.register_pretty_printer(gdb.current_objfile(), build_pretty_printer("Engine::mat2", "mat2t", r"mat2t<.*>", Mat2Printer))
gdb.printing.register_pretty_printer(gdb.current_objfile(), build_pretty_printer("Engine::mat3", "mat3t", r"mat3t<.*>", Mat3Printer))
gdb.printing.register_pretty_printer(gdb.current_objfile(), build_pretty_printer("Engine::mat4", "mat4t", r"mat4t<.*>", Mat4Printer))