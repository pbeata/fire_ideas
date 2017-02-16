"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class ifmOUT(object):
    __slots__ = ["currHRR"]

    def __init__(self):
        self.currHRR = 0.0

    def encode(self):
        buf = BytesIO()
        buf.write(ifmOUT._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">d", self.currHRR))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != ifmOUT._get_packed_fingerprint():
            raise ValueError("Decode error")
        return ifmOUT._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = ifmOUT()
        self.currHRR = struct.unpack(">d", buf.read(8))[0]
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if ifmOUT in parents: return 0
        tmphash = (0x152b5a5a6ff5997b) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if ifmOUT._packed_fingerprint is None:
            ifmOUT._packed_fingerprint = struct.pack(">Q", ifmOUT._get_hash_recursive([]))
        return ifmOUT._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)
