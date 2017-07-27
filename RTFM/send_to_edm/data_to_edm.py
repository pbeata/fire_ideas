"""LCM type definitions
This file automatically generated by lcm.
DO NOT MODIFY BY HAND!!!!
"""

try:
    import cStringIO.StringIO as BytesIO
except ImportError:
    from io import BytesIO
import struct

class data_to_edm(object):
    __slots__ = ["time_stamp", "num_rooms", "temperature", "O2_conc", "CO_conc", "CO2_conc", "HCN_conc", "heat_flux"]

    def __init__(self):
        self.time_stamp = 0.0
        self.num_rooms = 0
        self.temperature = []
        self.O2_conc = []
        self.CO_conc = []
        self.CO2_conc = []
        self.HCN_conc = []
        self.heat_flux = []

    def encode(self):
        buf = BytesIO()
        buf.write(data_to_edm._get_packed_fingerprint())
        self._encode_one(buf)
        return buf.getvalue()

    def _encode_one(self, buf):
        buf.write(struct.pack(">di", self.time_stamp, self.num_rooms))
        buf.write(struct.pack('>%dd' % self.num_rooms, *self.temperature[:self.num_rooms]))
        buf.write(struct.pack('>%dd' % self.num_rooms, *self.O2_conc[:self.num_rooms]))
        buf.write(struct.pack('>%dd' % self.num_rooms, *self.CO_conc[:self.num_rooms]))
        buf.write(struct.pack('>%dd' % self.num_rooms, *self.CO2_conc[:self.num_rooms]))
        buf.write(struct.pack('>%dd' % self.num_rooms, *self.HCN_conc[:self.num_rooms]))
        buf.write(struct.pack('>%dd' % self.num_rooms, *self.heat_flux[:self.num_rooms]))

    def decode(data):
        if hasattr(data, 'read'):
            buf = data
        else:
            buf = BytesIO(data)
        if buf.read(8) != data_to_edm._get_packed_fingerprint():
            raise ValueError("Decode error")
        return data_to_edm._decode_one(buf)
    decode = staticmethod(decode)

    def _decode_one(buf):
        self = data_to_edm()
        self.time_stamp, self.num_rooms = struct.unpack(">di", buf.read(12))
        self.temperature = struct.unpack('>%dd' % self.num_rooms, buf.read(self.num_rooms * 8))
        self.O2_conc = struct.unpack('>%dd' % self.num_rooms, buf.read(self.num_rooms * 8))
        self.CO_conc = struct.unpack('>%dd' % self.num_rooms, buf.read(self.num_rooms * 8))
        self.CO2_conc = struct.unpack('>%dd' % self.num_rooms, buf.read(self.num_rooms * 8))
        self.HCN_conc = struct.unpack('>%dd' % self.num_rooms, buf.read(self.num_rooms * 8))
        self.heat_flux = struct.unpack('>%dd' % self.num_rooms, buf.read(self.num_rooms * 8))
        return self
    _decode_one = staticmethod(_decode_one)

    _hash = None
    def _get_hash_recursive(parents):
        if data_to_edm in parents: return 0
        tmphash = (0x17f7de75b338b8a9) & 0xffffffffffffffff
        tmphash  = (((tmphash<<1)&0xffffffffffffffff)  + (tmphash>>63)) & 0xffffffffffffffff
        return tmphash
    _get_hash_recursive = staticmethod(_get_hash_recursive)
    _packed_fingerprint = None

    def _get_packed_fingerprint():
        if data_to_edm._packed_fingerprint is None:
            data_to_edm._packed_fingerprint = struct.pack(">Q", data_to_edm._get_hash_recursive([]))
        return data_to_edm._packed_fingerprint
    _get_packed_fingerprint = staticmethod(_get_packed_fingerprint)
