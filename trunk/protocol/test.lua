for_maker = {}
for_caller = {}

for_maker.c_simple = function (vfd, Id, Name)
	print(vfd, Id, Name)
end

proto.init_pto("for_maker", "for_caller")

local Id,Msg = proto.add_protocol("./pto/c_simple.pto")
local Id,Msg = proto.add_protocol("./pto/s_simple.pto")


for_caller.s_simple(9999, 1985, "zero")
