import matplotlib
matplotlib.use('Agg')  # For headless environments
import matplotlib.pyplot as plt

import sys

def parse_gmp(gmp_file):
    # Parse the gmp file to extract routing area, grid info, chips, bumps
    # The format (from problem statement):
    # .ra
    # <llx> <lly> <width> <height>
    # .g
    # <gridW> <gridH>
    # .c
    # <chip_llx> <chip_lly> <chip_w> <chip_h>
    # .b
    # <bumpIdx> <relX> <relY>
    # until empty line, then another chip ...
    
    RA = {}
    chips = []
    
    with open(gmp_file, 'r') as fin:
        lines = [l.strip() for l in fin.readlines()]
    
    idx = 0
    while idx < len(lines):
        line = lines[idx]
        if line == ".ra":
            idx += 1
            llx, lly, w, h = map(int, lines[idx].split())
            RA['llx'], RA['lly'], RA['w'], RA['h'] = llx, lly, w, h
        elif line == ".g":
            idx += 1
            # grid info not essential for plotting basic shapes, skip
        elif line == ".c":
            idx += 1
            chip_llx, chip_lly, chip_w, chip_h = map(int, lines[idx].split())
            idx += 1
            # Next should be .b or next chip
            bumps = []
            while idx < len(lines) and lines[idx].strip():
                if lines[idx].strip() == ".b":
                    idx += 1
                    # read bumps until empty line or next section
                    while idx < len(lines) and lines[idx].strip() and lines[idx][0] != '.':
                        parts = lines[idx].split()
                        bumpIdx = int(parts[0])
                        relX = int(parts[1])
                        relY = int(parts[2])
                        bumps.append((bumpIdx, relX, relY))
                        idx += 1
                else:
                    # might be next chip or empty line
                    break
            chips.append((chip_llx, chip_lly, chip_w, chip_h, bumps))
        idx += 1
    
    return RA, chips

def parse_lg(lg_file):
    # Parse the lg file to get routing results
    # Format:
    # n<idx>
    # via (maybe multiple times)
    # M1/M2 sx sy ex ey
    # .end
    # repeated for each net
    nets = []
    with open(lg_file, 'r') as fin:
        lines = [l.strip() for l in fin.readlines()]
    i = 0
    current_net = None
    current_segments = []
    while i < len(lines):
        line = lines[i]
        if line.startswith('n'):
            # start of a new net
            if current_net is not None:
                # store the previous net
                nets.append((current_net, current_segments))
            current_net = line
            current_segments = []
        elif line == 'via':
            current_segments.append(('via',))
        elif line == '.end':
            # end current net
            if current_net is not None:
                nets.append((current_net, current_segments))
                current_net = None
                current_segments = []
        else:
            # might be a wire
            # Format: M1 sx sy ex ey or M2 sx sy ex ey
            parts = line.split()
            if parts[0] in ('M1','M2'):
                layer = parts[0]
                sx, sy, ex, ey = map(int, parts[1:])
                current_segments.append((layer, sx, sy, ex, ey))
        i += 1
    
    return nets

def plot_design(RA, chips, nets, output_file):
    fig, ax = plt.subplots(figsize=(10,6))
    
    # Plot Routing area
    RA_rect = plt.Rectangle((RA['llx'], RA['lly']), RA['w'], RA['h'],
                            fill=False, edgecolor='#1f77b4', linewidth=2, label='Routing Area')
    ax.add_patch(RA_rect)
    
    # Plot chips and their bumps
    chip_colors = ['#2ca02c', '#ff7f0e']
    bump_colors = ['#d62728', '#9467bd']
    
    for c_i, c in enumerate(chips):
        chip_llx, chip_lly, chip_w, chip_h, bumps = c
        chip_rect = plt.Rectangle((RA['llx']+chip_llx, RA['lly']+chip_lly), 
                                  chip_w, chip_h, fill=False, 
                                  edgecolor=chip_colors[c_i % 2], linestyle='--', linewidth=2, 
                                  label=f'Chip {c_i+1}')
        ax.add_patch(chip_rect)
        
        # Plot bumps
        bx = []
        by = []
        for b in bumps:
            bumpIdx, relX, relY = b
            bx.append(RA['llx']+chip_llx+relX)
            by.append(RA['lly']+chip_lly+relY)
        if len(bx) > 0:
            ax.plot(bx, by, 'o', color=bump_colors[c_i % 2], label=f'Bumps Chip {c_i+1}')
    
    # Plot nets routing
    for net_name, segs in nets:
        for s in segs:
            if s[0] == 'via':
                continue
            else:
                layer, sx, sy, ex, ey = s
                ax.plot([sx, ex], [sy, ey], color='#b38f69', alpha=0.6, linewidth=2)
    
    # Plot vias
    for net_name, segs in nets:
        via_positions = []
        last_end = None
        for si, s in enumerate(segs):
            if s[0] == 'via':
                if last_end is not None:
                    via_positions.append(last_end)
            else:
                layer, sx, sy, ex, ey = s
                if si > 0 and segs[si-1][0] == 'via':
                    via_positions.append((sx, sy))
                last_end = (ex, ey)
        for vpos in via_positions:
            vx, vy = vpos
            ax.plot(vx, vy, 'x', color='black', markersize=8, label='Via')
    
    # Remove duplicate labels
    handles, labels = ax.get_legend_handles_labels()
    unique = dict(zip(labels, handles))
    ax.legend(unique.values(), unique.keys())

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_title('Routing Area and Chips with Bumps and Routing Results')
    ax.grid(True)

    plt.savefig(output_file, dpi=200)
    plt.close()


if __name__ == "__main__":
    # Usage: python plot.py <input.gmp> <input.lg> <output.png>
    # The code reads the gmp file for layout and the lg file for routing results,
    # then produces the png image.
    if len(sys.argv) < 4:
        print("Usage: python plot.py <input.gmp> <input.lg> <output.png>")
        sys.exit(1)
    
    gmp_file = sys.argv[1]
    lg_file = sys.argv[2]
    output_file = sys.argv[3]

    RA, chips = parse_gmp(gmp_file)
    nets = parse_lg(lg_file)

    plot_design(RA, chips, nets, output_file)
