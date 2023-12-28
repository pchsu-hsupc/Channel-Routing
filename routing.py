import os
import sys
import math
import matplotlib.pyplot as plt
import matplotlib.patches as patches

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
}

def parse_BoundaryList(f):
    lines = f.readlines() 
    num_lines = len(lines)

    for i, line in enumerate(lines):
        line = line.strip()
        if line[0] == 'T' or line[0] == 'B':
            parts = line.split()
            global_data['boundary_list'].append(Track(parts[0], int(parts[1]), int(parts[2])))
        elif i == num_lines - 2:
            global_data['topboundaryID'] = line.split()
        elif i == num_lines - 1:
            global_data['bottomboundaryID'] = line.split()


def parse_NetList(f):
    for line in f:
        if 'Channel' in line:
            parts = line.split()
            channel_density = int(parts[2])
            for i in range(channel_density + 1, 1, -1):
                track_label = f'C{i - 1}'
                start, end = 0, 0
                global_data['boundary_list'].append(Track(track_label, start, end))
        elif line[0] == 'C' or line[0] == 'T' or line[0] == 'B':
            parts = line.split()
            global_data['net_list'].append(Track(parts[0], int(parts[1]), int(parts[2])))


def plot():
    y_values_lookup = {}
    for i, track in enumerate(global_data['boundary_list']):
        plt.text(x=-0.3, y=0.5*i, s=track.n , va='center', ha='right', color='gray')
        plt.hlines(y=0.5*i, xmin=0, xmax=10, colors='gray', linestyles=':', lw=1)
        y_values_lookup[track.n] = 0.5*i
            
        if track.n[0] == 'T' or track.n[0] == 'B':
            boundaryIDs = global_data['topboundaryID'] if track.n[0] == 'T' else global_data['bottomboundaryID']
            text_va = 'bottom' if track.n[0] == 'T' else 'top'
            text_offset = 0.1 if track.n[0] == 'T' else -0.1
            plt.hlines(y=0.5*i, xmin=track.s, xmax=track.e, colors='black', lw=2)
            for x in range(track.s, track.e + 1): 
                plt.plot(x, 0.5*i, marker='o', color='black', markersize=4)
                plt.text(x, 0.5*i + text_offset, str(boundaryIDs[x]), color='red', ha='center', va=text_va)
    
    for i, track in enumerate(global_data['net_list']):
        if hasattr(track, 'n') and (track.n[0] == 'C'):
            plt.hlines(y=y_values_lookup[track.n], xmin=track.s, xmax=track.e, colors='blue', lw=2)
        if hasattr(track, 'n') and (track.n[0] == 'T' or track.n[0] == 'B'):
            plt.hlines(y=y_values_lookup[track.n], xmin=track.s, xmax=track.e, colors='blue', lw=2)

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
    plt.savefig(os.path.join(figures_dir, 'plot.png'))

def sort_tracks(track):
    prefix = track.n[0]
    num = int(track.n[1:])
    
    if prefix == 'T' or prefix == 'C':
        # For 'T' and 'C', sort by prefix in ascending order, then number in descending order
        return (prefix, num)
    elif prefix == 'B':
        # For 'B', sort by prefix in ascending order, then number in ascending order
        return (prefix, -num)


if __name__ == '__main__':

    if len(sys.argv) != 3:
        print("Usage: python routing.py input.in output.out")
        sys.exit(1)

    BoundaryList = sys.argv[1]
    NetList = sys.argv[2]

    with open(BoundaryList, 'r') as f:
        parse_BoundaryList(f)

    with open(NetList, 'r') as f:
        parse_NetList(f)
    
    global_data['boundary_list'].sort(key=sort_tracks)
    plot()

    print("Routing Done!")
    sys.exit(0)

    