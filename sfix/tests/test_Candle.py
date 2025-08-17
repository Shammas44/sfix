from sfix.client.client import *
from sfix.client.dictionary import *
from sfix.domain.Candle import *
import unittest
import time


def to_string(num: int) -> bytes:
    ##region
    value = str(num)
    return value.encode("utf-8")
    ##endregion


def create_pairs() -> Array[SFIX_KeyValue]:
    ##region
    pairs = (SFIX_KeyValue * 256)()
    pairs[Tag.TIMESTAMP.value] = SFIX_KeyValue(
        to_string(Tag.TIMESTAMP.value), to_string(int(time.time()))
    )
    pairs[Tag.DATE.value] = SFIX_KeyValue(
        to_string(Tag.DATE.value), "2024-01-01".encode("utf-8")
    )
    pairs[Tag.TIME.value] = SFIX_KeyValue(
        to_string(Tag.TIME.value), "12:00:00".encode("utf-8")
    )
    pairs[Tag.OPEN.value] = SFIX_KeyValue(to_string(Tag.OPEN.value), to_string(2))
    pairs[Tag.HIGH.value] = SFIX_KeyValue(to_string(Tag.HIGH.value), to_string(2))
    pairs[Tag.LOW.value] = SFIX_KeyValue(to_string(Tag.LOW.value), to_string(2))
    pairs[Tag.CLOSE.value] = SFIX_KeyValue(to_string(Tag.CLOSE.value), to_string(2))
    pairs[Tag.UP_VOLUME.value] = SFIX_KeyValue(
        to_string(Tag.UP_VOLUME.value), to_string(2)
    )
    pairs[Tag.DOWN_VOLUME.value] = SFIX_KeyValue(
        to_string(Tag.DOWN_VOLUME.value), to_string(2)
    )
    pairs[Tag.TOTAL_VOLUME.value] = SFIX_KeyValue(
        to_string(Tag.TOTAL_VOLUME.value), to_string(2)
    )
    pairs[Tag.UP_TICKS.value] = SFIX_KeyValue(
        to_string(Tag.UP_TICKS.value), to_string(2)
    )
    pairs[Tag.DOWN_TICKS.value] = SFIX_KeyValue(
        to_string(Tag.DOWN_TICKS.value), to_string(2)
    )
    pairs[Tag.TOTAL_TICKS.value] = SFIX_KeyValue(
        to_string(Tag.TOTAL_TICKS.value), to_string(2)
    )
    return pairs
    ##endregion


def expectCandle(candle: SFIX_Candle, t: unittest.TestCase) -> None:
    ##region
    t.assertEqual(candle.date.decode(), "2024-01-01")
    t.assertEqual(candle.time.decode(), "12:00:00")
    t.assertEqual(candle.close, 2)
    t.assertEqual(candle.open, 2)
    t.assertEqual(candle.high, 2)
    t.assertEqual(candle.low, 2)
    t.assertEqual(candle.upTicks, 2)
    t.assertEqual(candle.downTicks, 2)
    t.assertEqual(candle.totalTicks, 2)
    t.assertEqual(candle.upVolume, 2)
    t.assertEqual(candle.downVolume, 2)
    t.assertEqual(candle.totalVolume, 2)
    ##endregion


class SFIX_test_Candle(unittest.TestCase):

    def test_candle_constructor(self) -> None:
        ##region
        pairs = create_pairs()
        candle = sfix_candle_constructor(pairs)
        expectCandle(candle, self)
        ##endregion

    def test_to_sfix(self) -> None:
        ##region
        pairs = create_pairs()
        candle = sfix_candle_constructor(pairs)
        buffer = ctypes.create_string_buffer(SFIX_MSG_SIZE(13))
        candle.toSfix(candle, buffer)
        array = sfix_parse(buffer.value.decode("utf-8"))
        newCandle = sfix_candle_constructor(array)
        expectCandle(newCandle, self)
        ##endregion

    def test_validate(self) -> None:
        ##region
        pairs = create_pairs()
        res = sfix_candle_validate(pairs)
        self.assertEqual(res, True)
        ##endregion

    def test_invalid_date_format(self) -> None:
        ##region
        pairs = create_pairs()
        pairs[Tag.DATE.value] = SFIX_KeyValue(
            to_string(Tag.DATE.value), "01-01-2024".encode("utf-8")
        )
        res = sfix_candle_validate(pairs)
        self.assertEqual(res, False)
        ##endregion

    def test_invalid_time_format(self) -> None:
        ##region
        pairs = create_pairs()
        pairs[Tag.TIME.value] = SFIX_KeyValue(
            to_string(Tag.TIME.value), "08:00".encode("utf-8")
        )
        res = sfix_candle_validate(pairs)
        self.assertEqual(res, False)
        ##endregion

    def test_invalid_integer(self) -> None:
        ##region
        pairs = create_pairs()
        pairs[Tag.UP_VOLUME.value] = SFIX_KeyValue(
            to_string(Tag.UP_VOLUME.value), "0.1".encode("utf-8")
        )
        res = sfix_candle_validate(pairs)
        self.assertEqual(res, False)
        ##endregion

    def test_invalid_double(self) -> None:
        ##region
        pairs = create_pairs()
        pairs[Tag.OPEN.value] = SFIX_KeyValue(
            to_string(Tag.OPEN.value), "0,1".encode("utf-8")
        )
        res = sfix_candle_validate(pairs)
        self.assertEqual(res, False)
        ##endregion


if __name__ == "__main__":
    unittest.main()
