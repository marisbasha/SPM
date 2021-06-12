import numpy as np
import matplotlib.pyplot as plt

elems = {
    'SEQCPP': {},
    'PARCPP': {},
    'GRAPHCPP': {},
    'GRAPHCPP_FARM': {}
}


def push_to_elems(iteration, past, element):
    element = element.replace('\n', '')

    if not element in elems[past]:
        elems[past][element] = {'time': []}
    if not iteration in elems[past][element]:
        elems[past][element][iteration] = {'answer': []}


iteration = 1
past = ''
count = 0
f = open("log.txt", "r")
for x in f:
    s = x.split(':')
    if s[0] == 'SEQCPP' or s[0] == 'PARCPP' or s[0] == 'GRAPHCPP':
        past = s[0]
        if len(s) > 5:
            sx = s[5].replace('\n', '')
            if sx == '1':
                past = 'GRAPHCPP_FARM'
            else:
                past = 'GRAPHCPP'

        if s[0] == 'SEQCPP':
            iteration = 1
            element = s[1] + '_' + s[2] + '_' + s[3]
        else:
            element = s[2] + '_' + s[3] + '_' + s[4] + '_' + s[1]

        if s[0] == 'PARCPP':
            iteration += 1
            if iteration == 10:
                iteration = 1
            count += 1
        push_to_elems(iteration, past, element)
    else:
        l = x.split(' ')
        element = element.replace('\n', '')
        if l[0] == 'FF_TIME' or l[0] == 'PAR_TIME' or l[0] == 'SERIAL_TIME':
            elems[past][element]['time'].append(int(l[1]))
        elif l[0] == 'X':
            elems[past][element][iteration]['answer'].append(l[4])

f.close()


def getSeqTime(what, last):
    return elems[what][dt[0] + '_' + dt[1] + '_' + last]['time']


def get_parTimeN(what, where, last):
    return sum(elems[what][dt[0] + '_' + dt[1] + '_' + last + '_' + str(where)]['time']) / len(elems[what][dt[0] + '_' + dt[1] + '_' + last + '_' + str(where)]['time'])


def get_speedup(what, where, last):
    return elems['SEQCPP'][dt[0] + '_' + dt[1] + '_' + last]['time'][0] / get_parTimeN(what, where, last)


def getScalibility(what, where, last):
    return get_parTimeN(what, 1, last) / get_parTimeN(what, where, last)


def getEfficiency(what, where, last):
    return get_parTimeN(what, 1, last) / where * get_parTimeN(what, where, last)


cf = open("config", "r")
for c in cf:
    dt = c.split(':')
    last = dt[2].replace('\n', '')
    t = [1, 2, 4, 8, 16, 32, 64, 128, 256]
    seq = [getSeqTime('SEQCPP', last) for i in t]
    par = [get_parTimeN('PARCPP', i, last) for i in t]
    ff = [get_parTimeN('GRAPHCPP', i, last) for i in t]
    ff_f = [get_parTimeN('GRAPHCPP_FARM', i, last) for i in t]
    fig, ax = plt.subplots(figsize=(12, 6))
    plt.plot(t, seq, linewidth=1.5, label='Sequential')
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Performance with Graph of ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Time in milliseconds')
    plt.savefig('./images/performance_'+dt[0]+"_"+dt[1]+"_"+last+'.png')

    fig, ax = plt.subplots(figsize=(12, 6))
    par = [get_speedup('PARCPP', i, last) for i in t]
    ff = [get_speedup('GRAPHCPP', i, last) for i in t]
    ff_f = [get_parTimeN('GRAPHCPP_FARM', i, last) for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Speedup ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Speedup')
    plt.savefig('./images/speedup_'+dt[0]+"_"+dt[1]+"_"+last+'.png')

    fig, ax = plt.subplots(figsize=(12, 6))
    par = [get_speedup('PARCPP', i, last) for i in t]
    ff = [get_speedup('GRAPHCPP', i, last) for i in t]
    ff_f = [get_parTimeN('GRAPHCPP_FARM', i, last) for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Scalability ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Scalability')
    plt.savefig('./images/scalability_'+dt[0]+"_"+dt[1]+"_"+last+'.png')

    fig, ax = plt.subplots(figsize=(12, 6))
    par = [getEfficiency('PARCPP', i, last) for i in t]
    ff = [getEfficiency('GRAPHCPP', i, last) for i in t]
    ff_f = [get_parTimeN('GRAPHCPP_FARM', i, last) for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')
    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Efficiency ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Efficiency')
    plt.savefig('./images/efficiency_'+dt[0]+"_"+dt[1]+"_"+last+'.png')
