from pathlib import Path
import re
import unittest


class VpcSurfaceLifetimeTest(unittest.TestCase):
    def test_duplicate_attachment_is_rejected_before_allocation(self):
        source = (Path(__file__).parents[2] / "westeros-compositor.cpp").read_text()
        function = re.search(r"static void wstIVpcGetVpcSurface\([^;]*?\)\n\{.*?\n}\n", source, re.S)
        self.assertIsNotNone(function)
        body = function.group()
        guard = body.find("if ( surface->vpcSurface )")
        allocation = body.find("calloc(1,sizeof(WstVpcSurface))")
        assignment = body.find("surface->vpcSurface= vpcSurface")
        self.assertGreaterEqual(guard, 0)
        self.assertGreater(allocation, guard)
        self.assertGreater(assignment, allocation)
        self.assertIn("WL_DISPLAY_ERROR_INVALID_OBJECT", body[guard:allocation])


if __name__ == "__main__":
    unittest.main()
