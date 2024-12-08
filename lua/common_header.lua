-- Common Header of all packets
CommonHeader = {}

F_id      = ProtoField.uint16("Discover_hello.id", "ID", base.DEC)
F_counter = ProtoField.uint16("Discover_hello.counter", "MSG COUNTER", base.DEC)
F_flag    = ProtoField.uint8 ("Discover_hello.flag", "PROTOCOL FLAG", base.BIN)
F_subtype = ProtoField.uint8 ("Discover_hello.subtype", "SUBTYPE", base.DEC)
F_type    = ProtoField.uint8 ("Discover_hello.type", "TYPE", base.DEC)

function CommonHeader.define_fields()
    return {F_id, F_counter, F_flag, F_subtype, F_type}
end

function CommonHeader.getHeaderSubtype(buffer)
    return buffer(5, 1):uint()
end

function CommonHeader.dissect_common_header(buffer, subtree)
    -- Get all the common header fields
    local counter = buffer(0,2):le_uint()
    local id = buffer(2,2):le_uint()
    local type = buffer(4, 1):uint()
    local subtype = buffer(5, 1):uint()
    local flag = buffer(6, 1):uint()

    -- Add fields to the subtree
    subtree:add(F_counter, counter) -- MESSAGE COUNTER
    subtree:add(F_id, id)           -- MESSAGE ID
    subtree:add(F_type, type)       -- MESSAGE TYPE
    subtree:add(F_subtype, subtype) -- MESSAGE SUBTYPE
    subtree:add(F_flag, flag)       -- MESSAGE PROTOCOL FLAG

    -- Returns the total length of the header
    return 8
end