import os
import sys
import matplotlib.pyplot as plt


class Track:
    def __init__(self, n, s, e):
        self.n = n
        self.s = s
        self.e = e
class Dogleg:
    def __init__(self, p):
        self.p = p


global_data = {
    'boundary_list': [],
    'net_list': [],
    'topboundaryID': [],
    'bottomboundaryID': [],
    'max_length': 0
}
net_data = {}
y_values_lookup = {}


def calculate_dynamic_offset(x, net, y):
    i = 0
    for boundary in global_data['boundary_list']:
        if x in range(boundary.s, boundary.e + 1) and net == global_data['topboundaryID'][x] and boundary.n[0] == 'T':
            i = y_values_lookup[boundary.n] - y 
            break
        elif x in range(boundary.s, boundary.e + 1) and net == global_data['bottomboundaryID'][x] and boundary.n[0] == 'B':
            i = y - y_values_lookup[boundary.n]
            break

    return i

def parse_BoundaryList(f):
    lines = f.readlines() 
    num_lines = len(lines)

    for i, line in enumerate(lines):
        line = line.strip()
        if line[0] == 'T' or line[0] == 'B':
            parts = line.split()
            start, end = int(parts[1]), int(parts[2])
            if end > global_data['max_length']:
                global_data['max_length'] = end

            global_data['boundary_list'].append(Track(parts[0], int(parts[1]), int(parts[2])))
        elif i == num_lines - 2:
            global_data['topboundaryID'] = line.split()
        elif i == num_lines - 1:
            global_data['bottomboundaryID'] = line.split()

def parse_NetList(f):
    current_net = None
    for line in f:
        if 'Channel' in line:
            parts = line.split()
            channel_density = int(parts[2])
            for i in range(channel_density + 1, 1, -1):
                track_label = f'C{i - 1}'
                start, end = 0, 0
                global_data['boundary_list'].append(Track(track_label, start, end))
        elif 'Net' in line:
            parts = line.split()
            current_net = parts[1]
            net_data[current_net] = { 'doglegs': [], 'tracks': [] }
        elif current_net:
            if 'Dogleg' in line:
                parts = line.split()
                net_data[current_net]['doglegs'].append(Dogleg(int(parts[1])))
            elif line[0] == 'C' or line[0] == 'T' or line[0] == 'B':
                parts = line.split()
                global_data['net_list'].append(Track(parts[0], int(parts[1]), int(parts[2])))
                net_data[current_net]['tracks'].append(Track(parts[0], int(parts[1]), int(parts[2])))

def plot(number):
    plt.figure(figsize=(20, 20))
    y_cord = 0
    for i, track in enumerate(global_data['boundary_list']):
        if track.n not in y_values_lookup:
            plt.text(x=-0.3, y=y_cord, s=track.n , va='center', ha='right', color='gray', size=7)
            plt.hlines(y=y_cord, xmin=0, xmax=global_data['max_length'], colors='gray', linestyles=':', lw=1)
            y_values_lookup[track.n] = y_cord
            h = y_cord
            y_cord += 20
        else:
            h = y_values_lookup[track.n]
            
        if track.n[0] == 'T' or track.n[0] == 'B':
            boundaryIDs = global_data['topboundaryID'] if track.n[0] == 'T' else global_data['bottomboundaryID']
            text_va = 'bottom' if track.n[0] == 'T' else 'top'
            text_offset = 3 if track.n[0] == 'T' else - 3
            plt.hlines(y=h, xmin=track.s, xmax=track.e, colors='black', lw=2)
            for x in range(track.s, track.e + 1): 
                plt.plot(x, h, marker='o', color='black', markersize=4)
                plt.text(x, h + text_offset, str(boundaryIDs[x]), color='red', ha='center', va=text_va, size=6)
    
    for i, track in enumerate(global_data['net_list']):
        if hasattr(track, 'n') and (track.n[0] == 'C'):
            plt.hlines(y=y_values_lookup[track.n], xmin=track.s, xmax=track.e, colors='blue', lw=2)
        if hasattr(track, 'n') and (track.n[0] == 'T' or track.n[0] == 'B'):
            plt.hlines(y=y_values_lookup[track.n], xmin=track.s, xmax=track.e, colors='blue', lw=2)

    for net in net_data:
        for track in net_data[net]['tracks']:
            if hasattr(track, 'n') and track.n[0] in ('T', 'B', 'C'):
                for x in range(track.s, track.e + 1):
                    if net == global_data['bottomboundaryID'][x]:
                        dynamic_offset = calculate_dynamic_offset(x, net, y_values_lookup[track.n])
                        ymax = y_values_lookup[track.n]
                        ymin = y_values_lookup[track.n] - dynamic_offset
                        plt.vlines(x=x, ymin=ymin, ymax=ymax, colors='green', lw=1)
                    elif net == global_data['topboundaryID'][x]:
                        dynamic_offset = calculate_dynamic_offset(x, net, y_values_lookup[track.n])
                        ymax = y_values_lookup[track.n] + dynamic_offset
                        ymin = y_values_lookup[track.n]
                        plt.vlines(x=x, ymin=ymin, ymax=ymax, colors='green', lw=1)
  
        for dogleg in net_data[net]['doglegs']:
            for track in net_data[net]['tracks']:
                if hasattr(track, 'n') and track.n[0] in ('T', 'B', 'C'):
                    if track.e == dogleg.p:
                        ymax = y_values_lookup[track.n]
                    elif track.s == dogleg.p:
                        ymin = y_values_lookup[track.n]
            plt.vlines(x=dogleg.p, ymin=ymin, ymax=ymax, colors='red', lw=2)

    # Remove the axes
    plt.gca().spines['top'].set_visible(False)
    plt.gca().spines['right'].set_visible(False)
    plt.gca().spines['left'].set_visible(False)
    plt.gca().spines['bottom'].set_visible(False)
    
    # Remove the ticks
    plt.gca().set_xticks([])
    plt.gca().set_yticks([])

    figures_dir = os.path.join(os.path.dirname(__file__), 'figures')
    os.makedirs(figures_dir, exist_ok=True)
    plt.savefig(os.path.join(figures_dir, f'plot{number}.png'), dpi=800)

def sort_tracks(track):
    prefix = track.n[0]
    num = int(track.n[1:])
    
    if prefix == 'T' or prefix == 'C':
        # For 'T' and 'C', sort by prefix in ascending order, then number in descending order
        return (prefix, num)
    elif prefix == 'B':
        # For 'B', sort by prefix in ascending order, then number in ascending order
        return (prefix, -num)


def main():
    if len(sys.argv) != 4:
        print("Usage: python3 routing.py input<number>.in output<number>.out <number>")
        sys.exit(1)

    BoundaryList = sys.argv[1]
    NetList = sys.argv[2]
    Number = sys.argv[3]

    with open(BoundaryList, 'r') as f:
        parse_BoundaryList(f)

    with open(NetList, 'r') as f:
        parse_NetList(f)
    
    global_data['boundary_list'].sort(key=sort_tracks)
    plot(Number)

    print("Routing Done!")


if __name__ == '__main__':

    main()

    