from pathlib import Path
import re
import unittest


class FlushLifetimeTest(unittest.TestCase):
    def test_freed_surface_memory_is_cleared(self):
        source = (Path(__file__).parents[2] / "westeros-render-gl.cpp").read_text()
        function = re.search(r"static void wstRendererGLFlushSurface\([^;]*?\)\n\{.*?\n}\n", source, re.S)
        self.assertIsNotNone(function)
        self.assertRegex(function.group(), r"free\(\s*surface->mem\s*\);\s*surface->mem\s*=\s*0\s*;")


if __name__ == "__main__":
    unittest.main()
