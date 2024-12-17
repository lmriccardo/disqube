-- dofile("C:/Users/ricca/Desktop/disqube/lua/discover_response.lua")

Discover_response = Proto("Discover_response", "DISCOVER RESPONSE")

-- Defining the fields for the protocol
local resp_udp_prt = ProtoField.uint16("Discover_response.udp_prt", "UDP PORT", base.DEC)
local resp_tcp_prt = ProtoField.uint16("Discover_response.tcp_prt", "TCP PORT", base.DEC)
local resp_ip_addr = ProtoField.uint32("Discover_response.ip_addr", "SRC IP ADDRESS", base.hex)
local resp_free_ram_mb = ProtoField.uint32("Discover_response.free_ram_mb", "FREE RAM [MB]", base.DEC)
local resp_free_ram_kb = ProtoField.uint32("Discover_response.free_ram_kb", "FREE RAM [KB]", base.DEC)
local resp_cpu_usage = ProtoField.uint8("Discover_response.cpu_usage", "CPU USAGE %", base.DEC)

Discover_response.fields = {
    F_id, F_counter, F_flag, F_subtype, F_type, resp_udp_prt, resp_tcp_prt,
    resp_ip_addr, resp_free_ram_mb, resp_free_ram_kb, resp_cpu_usage
}

-- Dissector Function
function Discover_response.dissector(buffer, pinfo, tree)
    -- Check the buffer has enough length
    if buffer:len() < 32 and buffer:len() > 32 then
        return
    end

    -- Check that the subtype is DISCOVER RESPONSE
    if CommonHeader.getHeaderSubtype(buffer) ~= 2 then
        return
    end

    pinfo.cols.protocol = "DISCOVER RESPONSE"

    local subtree = tree:add(Discover_response, buffer(), "Discover Response Data")

    -- Dissect the Common header
    local remain_len = CommonHeader.dissect_common_header(buffer, subtree)

    -- Get the Discover response data
    local udp_prt = buffer(remain_len, 2):le_uint()
    local tcp_prt = buffer(remain_len + 2, 2):le_uint()
    local ip_addr = buffer(remain_len + 4, 4):le_uint()
    local ram_mb = buffer(remain_len + 8, 4):le_uint()
    local ram_kb = buffer(remain_len + 12, 4):le_uint()
    local cpu_usage = buffer(remain_len + 16, 1):uint()

    subtree:add(resp_udp_prt, buffer(remain_len, 2), udp_prt)          -- MESSAGE DATA: UDP PORT
    subtree:add(resp_tcp_prt, buffer(remain_len + 2, 2), tcp_prt)      -- MESSAGE DATA: TCP PORT
    subtree:add(resp_ip_addr, buffer(remain_len + 4, 4), ip_addr)      -- MESSAGE DATA: SRC IP ADDRESS
    subtree:add(resp_free_ram_mb, buffer(remain_len + 8, 4), ram_mb)   -- MESSAGE DATA: FREE RAM [MB]
    subtree:add(resp_free_ram_kb, buffer(remain_len + 12, 4), ram_kb)   -- MESSAGE DATA: FREE RAM [KB]
    subtree:add(resp_cpu_usage, buffer(remain_len + 16, 1), cpu_usage) -- MESSAGE DATA: CPU USAGE
end

local udp = DissectorTable.get("udp.port")
udp:add(32126, Discover_response)
