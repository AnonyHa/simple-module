for_maker = {}
for_caller = {}

for_maker.c_simple = function (vfd, Id, Name)
	print(vfd, Id, Name)
end

local ProtoManager = pto.object:new()
print(ProtoManager)

print(ProtoManager:add_protocol("./pto/c_simple.pto"))
print(ProtoManager:add_protocol("./pto/s_simple.pto"))

for_caller.s_simple(9999, 1985, "zero")
