for_maker = {}
for_caller = {}

for_maker.c_simple = function (vfd, Id, Name)
	print(vfd, Id, Name)
end

local ProtoManager = pto.object.GetProtoObj("cishi")
print(ProtoManager:add_protocol("./pto/c_simple.pto"))
print(ProtoManager:add_protocol("./pto/s_simple.pto"))

for_caller.s_simple(9999, 1985, "zero")

print("======================================================")

for_maker_test = {}
for_caller_test = {}

for_maker_test.c_simple_test = function(vfd, Id, Name)
	print("Another Pto", vfd, Id, Name)
end

local ProtoManager1 = pto.object.GetProtoObj("cishi_test")
print(ProtoManager1:add_protocol("./pto/c_simple_test.pto"))
print(ProtoManager1:add_protocol("./pto/s_simple_test.pto"))

for_caller_test.s_simple_test(8888, 1985, "zero")
