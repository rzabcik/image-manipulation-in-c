"""
This script generates a profile_runner.sh script that runs the profile executables multiple times and combines the output in "times.csv"
"""

# config
image_path = ""
images = ["constitution.png"]
repetitions = 10
ops = ["./profile_rotate", "./profile_median_filter"]
output_filename = "times.csv"

print "#!/bin/sh"
print "echo \"operation,filename,width,height,time_ns\" > %s" % output_filename

i = 0
num_ops = len(ops) * len(images) * repetitions

for im in images:
    for _ in xrange(repetitions):
        for op in ops:
            print "%s %s%s >> %s" % (op, image_path, im, output_filename)
            i += 1
            print "echo \"%d of %d runs completed (%s %s).\"" % (i, num_ops, op, im)

print "echo \"Done. Output in %s\"" % output_filename
