from pathlib import Path
import re
import unittest


class NestedBufferLifetimeTest(unittest.TestCase):
    def test_remote_destroy_clears_pointer(self):
        source = (Path(__file__).parents[2] / "westeros-nested.cpp").read_text()
        function = re.search(r"static void buffer_remote_destroy_notify\([^;]*?\)\n\{.*?\n}\n", source, re.S)
        self.assertIsNotNone(function)
        self.assertIn("binfo->bufferRemote= NULL", function.group())

    def test_deferred_cleanup_removes_listener_before_free(self):
        source = (Path(__file__).parents[2] / "westeros-nested.cpp").read_text()
        function = re.search(r"void WstNestedConnectionReleaseRemoteBuffers\([^;]*?\)\n\{.*?\n}\n", source, re.S)
        self.assertIsNotNone(function)
        body = function.group()
        remove = body.find("wl_list_remove( &binfo->bufferRemoteDestroyListener.link )")
        release = body.find("free( binfo )")
        self.assertGreaterEqual(remove, 0)
        self.assertGreater(release, remove)


if __name__ == "__main__":
    unittest.main()
