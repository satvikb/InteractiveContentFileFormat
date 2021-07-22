var fs = require('fs');
var assert = require('assert');
var BYTES;
var i = 0;
function readFile(filename){
  let fileData = fs.readFileSync(filename).toString('hex');
  BYTES = fileData.toUpperCase()
  console.log(typeof fileData)
  readMagicNumber()
  readHeaderChunk()

  while(i < BYTES.length){
    readChunk()
  }
}

function toInt(hex){
  return parseInt(Number("0x"+hex), 10)
}


function readChunk(){
  let typeID = readTypeAndID();

  switch (typeID[0]) {
    case 1:
      // container
      break;
    default:

  }
}

function readContainer(){

}

function readTypeAndID(){
  const firstByte = BYTES.substring(i, i+2);
  const num = toInt(firstByte)
  var type = num >> 5 & 0x7;
  var id;
  if(type == 0x6){
    // extended
    type = num & 0x1F;

  }else{
    const secondByte = BYTES.substring(i+2, i+4);

    console.log(firstByte+secondByte)
    id = toInt(firstByte+secondByte)
    i += 4;
  }

  return [type, id]
}

function printTypeAndID(typeAndID){
    console.log("Type: "+typeAndID[0]+" ID:"+typeAndID[1])
}

function readMagicNumber(){
  const expectedMagicNum = "73616D616C69746C6E767561"
  assert(BYTES.substring(i, expectedMagicNum.length) == expectedMagicNum, "Invalid Magic Number")
  i += expectedMagicNum.length;
}

function readHeaderChunk(){
  const exepectedID = "C8000000"
  assert(BYTES.substring(i, i+exepectedID.length) == exepectedID)
  i += exepectedID.length;

  const exepectedVersion = "0001"
  assert(BYTES.substring(i, i+exepectedVersion.length) == exepectedVersion)
  i += exepectedVersion.length;

  const expectedStarting = "2001"
  assert(BYTES.substring(i, i+expectedStarting.length) == expectedStarting)
  i += expectedStarting.length;

  // read key pairs
}

module.exports = {
  readFile: readFile
}
