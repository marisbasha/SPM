import numpy as np


f = open("./data/logs/output_time.txt", "r")
string = ''
for x in f:
    s = x.split(':')
    if len(s) > 1:
        string += '\\\\'
        print(string)
        string = 'Node \ ' + str(s[0]) + '.Edges \ min \ ' + \
            str(s[1]) + ' - max \ ' + \
            str(sum([int(s[1]), int(s[2])])) + '& \ ' + str(s[3])
    else:
        if 'AVG' not in x:
            if x is not 0:
                string += '&' + str(x) + ' usec'
            else:
                string += '& N/A'
