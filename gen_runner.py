"""
This script generates a profile_runner.sh script that runs the profile executables multiple times and combines the output in "times.csv"
"""

# config
image_path = "/var/local/test_pngs/"
images = [
    "circle4.png",
    "martian_colour_wheel_24_hue_f.png",
    "resnik.png",
    "watermelon_PNG232.png",
    "what_even.png",
]
repetitions = 10
ops = [
    "./profile_rotate",
    "./profile_median_filter",
]
output_filename = "times.csv"

print "#!/bin/sh"
print "echo \"operation,filename,width,height,time_ns,n_threads\" > %s" % output_filename

def regular_test():
    i = 0
    num_ops = len(ops) * len(images) * repetitions

    for im in images:
        for _ in xrange(repetitions):
            for op in ops:
                print "%s %s%s >> %s" % (op, image_path, im, output_filename)
                i += 1
                print "echo \"%d of %d runs completed (%s %s).\"" % (i, num_ops, op, im)

    print "echo \"Done. Output in %s\"" % output_filename

def rotate_scalability_test():
    op = "./profile_rotate"
    import multiprocessing
    cores = multiprocessing.cpu_count()

    i = 0
    num_ops = len(images) * repetitions * cores
    for im in images:
        for _ in xrange(repetitions):
            for n_threads in xrange(1,cores+1):
                print "%s %s%s %d >> %s" % (op, image_path, im, n_threads, output_filename)
                i += 1
                print "echo \"%d of %d runs completed (%s %s %d).\"" % (i, num_ops, op, im, n_threads)

# regular_test()
rotate_scalability_test()
