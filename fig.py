import numpy as np
import matplotlib.pyplot as plt

elems = {
    'SEQCPP': {},
    'PARCPP': {},
    'GRAPHCPP': {},
    'GRAPHCPP_FARM': {},
    'SEQCPP_NT': {},
    'PARCPP_NT': {},
    'GRAPHCPP_NT': {},
    'GRAPHCPP_FARM_NT': {}
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
f = open("./data/logs/log_s.txt", "r")
for x in f:
    s = x.split(':')
    if s[0] == 'SEQCPP' or s[0] == 'PARCPP' or s[0] == 'GRAPHCPP' or s[0] == 'SEQCPP_NT' or s[0] == 'PARCPP_NT' or s[0] == 'GRAPHCPP_NT':
        past = s[0]
        if len(s) > 5:
            sx = s[5].replace('\n', '')
            if s[0] == 'GRAPHCPP_NT':
                if sx == '1':
                    past = 'GRAPHCPP_FARM_NT'
                else:
                    past = 'GRAPHCPP_NT'
            elif s[0] == 'GRAPHCPP':
                if sx == '1':
                    past = 'GRAPHCPP_FARM'
                else:
                    past = 'GRAPHCPP'

        if s[0] == 'SEQCPP' or s[0] == 'SEQCPP_NT':
            iteration = 1
            element = s[1] + '_' + s[2] + '_' + s[3]
        else:
            element = s[2] + '_' + s[3] + '_' + s[4] + '_' + s[1]

        if s[0] == 'PARCPP' or s[0] == 'PARCPP_NT':
            iteration += 1
            if iteration == 10:
                iteration = 1
            count += 1
        push_to_elems(iteration, past, element)
    else:
        l = x.split(' ')
        element = element.replace('\n', '')
        if l[0] == 'FF_TIME' or l[0] == 'PAR_TIME' or l[0] == 'SERIAL_TIME' or l[0] == 'FF_TIME_NT' or l[0] == 'PAR_TIME_NT' or l[0] == 'SERIAL_TIME_NT':
            elems[past][element]['time'].append(int(l[1]))
        elif l[0] == 'X':
            elems[past][element][iteration]['answer'].append(l[4])

f.close()

print(elems)


def getSeqTime(what, last):
    return sum(elems[what][dt[0] + '_' + dt[1] + '_' + last]['time']) / len(elems[what][dt[0] + '_' + dt[1] + '_' + last]['time'])


def get_parTimeN(what, where, last):
    return sum(elems[what][dt[0] + '_' + dt[1] + '_' + last + '_' + str(where)]['time']) / len(elems[what][dt[0] + '_' + dt[1] + '_' + last + '_' + str(where)]['time'])


def get_speedup(seq, what, where, last):
    return getSeqTime(seq, last) / get_parTimeN(what, where, last)


def getScalibility(what, where, last):
    return get_parTimeN(what, 1, last) / get_parTimeN(what, where, last)


def getEfficiency(what, where, last):
    return get_parTimeN(what, 1, last) / where * get_parTimeN(what, where, last)


cf = open("./data/config", "r")
for c in cf:
    dt = c.split(':')
    last = dt[2].replace('\n', '')
    #t = [1, 2, 4, 8, 16, 32, 64, 128, 256]
    t = [1, 2, 4, 8, 16, 32, 64]
    # With time
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
    plt.savefig('./data/images/performance_'+dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################

    # Without time
    seq = [getSeqTime('SEQCPP_NT', last) / 1000 for i in t]
    par = [get_parTimeN('PARCPP_NT', i, last) / 1000 for i in t]
    ff = [get_parTimeN('GRAPHCPP_NT', i, last) / 1000 for i in t]
    ff_f = [get_parTimeN('GRAPHCPP_FARM_NT', i, last) / 1000 for i in t]
    fig, ax = plt.subplots(figsize=(12, 6))
    plt.plot(t, seq, linewidth=1.5, label='Sequential')
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Performance with active wait with Graph of ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Time in milliseconds')
    plt.savefig('./data/images/performance_nt_' +
                dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################

    # With time
    fig, ax = plt.subplots(figsize=(12, 6))
    par = [get_speedup('SEQCPP', 'PARCPP', i, last) for i in t]
    ff = [get_speedup('SEQCPP', 'GRAPHCPP', i, last) for i in t]
    ff_f = [get_speedup('SEQCPP', 'GRAPHCPP_FARM', i, last) for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Speedup with active wait ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Speedup')
    plt.savefig('./data/images/speedup_'+dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################

    # Without time
    fig, ax = plt.subplots(figsize=(12, 6))
    par = [get_speedup('SEQCPP_NT', 'PARCPP_NT', i, last) / 1000 for i in t]
    ff = [get_speedup('SEQCPP_NT', 'GRAPHCPP_NT', i, last) / 1000 for i in t]
    ff_f = [get_speedup('SEQCPP_NT', 'GRAPHCPP_FARM_NT',
                        i, last) / 1000 for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Speedup ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Speedup')
    plt.savefig('./data/images/speedup_nt_'+dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################

    # With time
    fig, ax = plt.subplots(figsize=(12, 6))
    par = [getScalibility('PARCPP', i, last) for i in t]
    ff = [getScalibility('GRAPHCPP', i, last) for i in t]
    ff_f = [getScalibility('GRAPHCPP_FARM', i, last) for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Scalability with active wait ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Scalability')
    plt.savefig('./data/images/scalability_'+dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################

    # Without time
    fig, ax = plt.subplots(figsize=(12, 6))
    par = [getScalibility('PARCPP_NT', i, last) / 1000for i in t]
    ff = [getScalibility('GRAPHCPP_NT', i, last) / 1000 for i in t]
    ff_f = [getScalibility('GRAPHCPP_FARM_NT', i, last) / 1000 for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Scalability ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Scalability')
    plt.savefig('./data/images/scalability_nt_' +
                dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################

    # Without time
    fig, ax = plt.subplots(figsize=(12, 6))
    par = [getEfficiency('PARCPP', i, last) for i in t]
    ff = [getEfficiency('GRAPHCPP', i, last) for i in t]
    ff_f = [getEfficiency('GRAPHCPP_FARM', i, last) for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')
    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Efficiency with active wait ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Efficiency')
    plt.savefig('./data/images/efficiency_'+dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################

    # With time
    fig, ax = plt.subplots(figsize=(12, 6))
    par = [getEfficiency('PARCPP_NT', i, last) / 1000for i in t]
    ff = [getEfficiency('GRAPHCPP_NT', i, last) / 1000 for i in t]
    ff_f = [getEfficiency('GRAPHCPP_FARM_NT', i, last) / 1000 for i in t]
    plt.plot(t, par, linewidth=1.5, label='Parallel')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm')
    ax.grid(True)
    ax.legend(loc='right')
    ax.set_title('Efficiency ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Efficiency')
    plt.savefig('./data/images/efficiency_'+dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################

    # Comparison
    seq = [getSeqTime('SEQCPP', last) for i in t]
    par = [get_parTimeN('PARCPP', i, last) for i in t]
    ff = [get_parTimeN('GRAPHCPP', i, last) for i in t]
    ff_f = [get_parTimeN('GRAPHCPP_FARM', i, last) for i in t]
    seq_nt = [getSeqTime('SEQCPP_NT', last) / 1000 for i in t]
    par_nt = [get_parTimeN('PARCPP_NT', i, last) / 1000 for i in t]
    ff_nt = [get_parTimeN('GRAPHCPP_NT', i, last) / 1000 for i in t]
    ff_f_nt = [get_parTimeN('GRAPHCPP_FARM_NT', i, last) / 1000 for i in t]
    fig, ax = plt.subplots(figsize=(12, 6))
    plt.plot(t, seq, linewidth=1.5, label='Sequential active wait')
    plt.plot(t, par, linewidth=1.5, label='Parallel active wait')
    plt.plot(t, ff, linewidth=1.5, label='Fastflow active wait')
    plt.plot(t, ff_f, linewidth=1.5, label='Fastflow Farm active wait')
    plt.plot(t, seq_nt, linewidth=1.5, label='Sequential')
    plt.plot(t, par_nt, linewidth=1.5, label='Parallel')
    plt.plot(t, ff_nt, linewidth=1.5, label='Fastflow')
    plt.plot(t, ff_f_nt, linewidth=1.5, label='Fastflow Farm')

    ax.grid(True)
    ax.legend(loc='right')
    ax.set_yscale('log')
    ax.set_title('Performance comparison with and without active wait with Graph of ' +
                 dt[0] + ' nodes with min edges:' + dt[1] + ' and max edges:' + dt[2])
    ax.set_xlabel('N. threads')
    ax.set_ylabel('Time in milliseconds')
    plt.savefig('./data/images/performance_comp_' +
                dt[0]+"_"+dt[1]+"_"+last+'.png')
    ##########################
