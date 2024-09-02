from sfix.sfix import *
from sfix.dictionary import Tag
import unittest


def to_string(num: int):
    ##region
    value = str(num)
    return value.encode("utf-8")
    ##endregion

def create_pairs():
    ##region
    pairs = (SFIX_Pair * 4)()
    pairs[0] = SFIX_Pair(Tag.HIGH.value, to_string(2))
    pairs[1] = SFIX_Pair(Tag.OPEN.value, to_string(2))
    pairs[2] = SFIX_Pair(Tag.CLOSE.value, to_string(2))
    pairs[3] = SFIX_Pair(Tag.LOW.value, to_string(2))
    return pairs
    ##endregion

class SFIX_test(unittest.TestCase):

    def test_unknown_msg(self):
        ##region
        msg = sfix_unknown()
        expected = "8=SFIX.1\x019=000\x0135=U\x01\x02"
        out = sfix_parse(msg)
        self.assertEqual(msg, expected)
        self.assertEqual(out[Tag.MESSAGE_TYPE.value].value.decode(), "U")
        ##endregion

    def test_acknowledge_msg(self):
        ##region
        msg = sfix_acknowledge()
        expected = "8=SFIX.1\x019=000\x0135=A\x01\x02"
        out = sfix_parse(msg)
        self.assertEqual(msg, expected)
        self.assertEqual(out[Tag.MESSAGE_TYPE.value].value.decode(), "A")
        ##endregion

    def test_pretty_print(self):
        ##region
        msg = sfix_acknowledge()
        sfix_print(msg)
        ##endregion

    def test_single_message(self):
        ##region
        pairs = create_pairs()
        buffer = SFIX_createBuffer(4)
        msg = sfix_compose(buffer, "O", pairs)
        out = sfix_parse(msg)
        self.assertEqual(out[Tag.MESSAGE_TYPE.value].value.decode(), "O")
        self.assertEqual(out[Tag.HIGH.value].value.decode(), "2")
        self.assertEqual(out[Tag.OPEN.value].value.decode(), "2")
        self.assertEqual(out[Tag.CLOSE.value].value.decode(), "2")
        self.assertEqual(out[Tag.LOW.value].value.decode(), "2")
        ##endregion

    def test_multiple_message(self):
        ##region
        length = 2
        buffer = SFIX_createBuffer(4 * length)
        msg = ""
        for i in range(length):
            pairs = create_pairs()
            msg = sfix_compose(buffer, "O", pairs)
        out = sfix_parse(msg)
        # self.assertEqual(out[Tag.MESSAGE_TYPE.value].value.decode(), "O")
        # self.assertEqual(out[Tag.HIGH.value].value.decode(), "2")
        # self.assertEqual(out[Tag.OPEN.value].value.decode(), "2")
        # self.assertEqual(out[Tag.CLOSE.value].value.decode(), "2")
        # self.assertEqual(out[Tag.LOW.value].value.decode(), "2")
        ##endregion


if __name__ == "__main__":
    unittest.main()


# def sfix_listLength(msg:str):
