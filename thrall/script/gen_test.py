#coding = utf8

f = open("../data/push.log")

index = 0

put = '{"api":"gettestso","cmd":"query","uid":'
for line in f:
	
	line = line.strip()
	output = put + line + "}"

	print output
	index = index + 1

f.close()
