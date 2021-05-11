# importing the module
import json

# reading the data from the file
with open('dictionary.json') as f:
    data = f.read()

print("Data type before reconstruction : ", type(data))

# reconstructing the data as a dictionary
js = json.loads(data)

print("Data type after reconstruction : ", type(js))
print(len(js))

def decToHex(val, dig = 2):
    v = hex(val)
    v = v[2:]
    return v.zfill(dig)

byLetter = {}
for i in js:
    val=js[i]
    if i[0] not in byLetter:
        byLetter[i[0]] = {}
    byLetter[i[0]][i] = val
   # print(i[0])

print(list(byLetter.keys()))

header = "7361746200012001417574686F72204E616D6500"
#############
# ----1-----
# @@2@@**3**
# @@@@@*****
topLayout = "6001 03 0000640A 0000 00   000A325A 0000 00   320A325A 0000 00"
topContainer = "2001 6001 4001 FF 4001 FF 4001  FF 00"

# define the content for the 26 letters w/ links at the top
tocContent = "4001 01"
tocTextContent = ""
i = 1
lets = list(byLetter.keys())
lets.sort()
for k in lets:
    tocTextContent += "EEBC82"
    tocTextContent += "80"+decToHex(i)
    tocTextContent += format(ord(k), "x")
    tocTextContent += "EEBC83"
    tocTextContent += format(ord(" "), "x") # test
    i += 1

tocContent += decToHex(len(tocTextContent), 8)+""
tocContent += tocTextContent

print(tocContent)

final = header + topLayout + topContainer + tocContent

data = bytearray.fromhex(final)
f = open('example_dictionary.ic', 'wb')
f.write(data)
f.close()
# # no support for infinite containers
# def createContainer(id,layoutID,contentIDs):
#     ret = id
#     ret += layoutID
#     for k in contentIDs:
#         ret += k
#         ret += "FF"
#     ret += "00"
#     return ret
#
# def createLayout(id,ele,)
