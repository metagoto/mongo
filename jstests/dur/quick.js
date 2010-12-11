/* quick.js
   test durability
   this file should always run quickly
   other tests can be slow
*/

print("quick.js");

// directories
var path1 = "dur1testnodur";
var path2 = "dur1testdur";

var step = 1;
function log(str) {
    if(str)
        print("step " + step++ + " " + str);
    else
        print("step " + step++);
}

//stopMongo(30000, 9);

// non-durable version
log();
var conn = startMongodEmpty("--port", 30000, "--dbpath", path1);
log();
var d = conn.getDB("test");
d.foo.insert({ x: 1 });
log();
stopMongod(30000);

// durable version
log();
var conn = startMongodEmpty("--port", 30001, "--dbpath", path2, "--dur");
log();
var d = conn.getDB("test");
d.foo.insert({ x: 1 });
log();

// wait for group commit.  use getLastError(...) later when that is enhanced.
sleep(400);

// kill the process hard
stopMongod(30001, /*signal*/9);

// journal file should be present, and non-empty as we killed hard

// restart and recover
log();
var conn = startMongodNoReset("--port", 30002, "--dbpath", path2, "--dur");
log();
var d = conn.getDB("test");
print("count:" + d.foo.count());
assert(d.foo.count() == 1);

log("stop");
stopMongod(30002);

// at this point, after clean shutdown, there should be no journal files
log("check no journal files");
assert(ls(path2 + "/journal") == null);

log("check data matches");
var diff = run("diff", path1 + "/test.ns", path2 + "/test.ns");
print(diff);
assert(diff == "", "error test.ns files differ");
var diff = run("diff", path1 + "/test.0", path2 + "/test.0");
print(diff);
assert(diff == "", "error test.0 files differ");

print("SUCCESS dur1.js");
