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

def stringToHexStr(s):
     return "".join("{:02x}".format(ord(c)) for c in s)
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
topContainer = "2001 6001 4001 FF 4001 FF 4A00  FF 00"

# define the content for the 26 letters w/ links at the top
tocContent = "4001 01"
tocTextContent = ""
i = 1
lets = list(byLetter.keys())
lets.sort()

replacementActions = []

# content ID 40 02 - (h4002 + d26) is for the list of words for each letter
for k in lets:
    tocTextContent += "EEBC82"
    repID = "80"+decToHex(i)
    tocTextContent += repID
    tocTextContent += format(ord(k), "x")
    tocTextContent += "EEBC83"
    tocTextContent += format(ord(" "), "x") # test

    repAct = repID+"03 2001 01 40"+decToHex(i+1)
    replacementActions.append(repAct)

    i += 1

print("FIVEOFOUR "+str(len(tocTextContent))+"_"+tocTextContent)
tocContent += decToHex(int(len(tocTextContent)/2), 8)+""
tocContent += tocTextContent

# create the list of words for each letter (content id 4002 - 4002+26)
letterWordLists = []
i = 1
totByte = 0
for k in lets:
    contentID = "40"+decToHex(i+1)
    type = "01"

    wordListContent = ""
    for word in byLetter[k]:
        wordListContent += stringToHexStr(word+"\n")
        #print(word+" "+stringToHexStr(word))
    conLenNum = int(len(wordListContent)/2)
    contentLen = decToHex(conLenNum, 8)+""
    print(k+": "+contentLen+" "+str(conLenNum)+" bytes "+contentID)
    totByte += conLenNum
    letterWordLists.append(contentID + type + contentLen + wordListContent)
    i += 1

print(totByte);
finalActions = "".join(replacementActions)
finalWordLists = "".join(letterWordLists)

imageContent = "4A00 02"
imageMetadata = "01 012C 00E0 00 02 03 32003264 04"
#imageMetadata = "01 012C 00E0 00 02 03 04"
from PIL import Image
im = Image.open('example.jpg') # Can be many different formats.
pix = im.load()
width, height = im.size
imagePixelData = ""
for y in range(height):
    for x in range(width):
        r,g,b = im.getpixel((x,y))
        r = decToHex(r)
        g = decToHex(g)
        b = decToHex(b)
        imagePixelData += r
        imagePixelData += g
        imagePixelData += b

finalImageContentData = imageMetadata + imagePixelData
imageContentLengthRaw = int(len(finalImageContentData)/2)
imageContentLength = decToHex(imageContentLengthRaw, 8)+""
imageContent += imageContentLength
finalImage = imageContent + finalImageContentData
#print("FIN "+finalImage)
final = header + topLayout + topContainer + tocContent + finalActions + finalWordLists + finalImage

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
