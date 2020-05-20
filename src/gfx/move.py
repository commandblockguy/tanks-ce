import os

for n in range(0,9):
    in_name = 'rendered/%04d.png' % (n)
    out_name = 'player_base/pl_base_%d.png' % (n)
    os.rename(in_name, out_name)

for n in range(0, 9):
    in_name = 'rendered/%04d.png' % (n + 9)
    out_name = 'player_turret/pl_turret_%d.png' % (n)
    os.rename(in_name, out_name)