import colorsys
import bitstruct


def colorize(base_hls, target_hls):
    base_h, base_l, base_s = base_hls
    target_h, target_l, target_s = target_hls
    # stolen from the GIMP source, probably not used correctly:
    # https://github.com/GNOME/gimp/blob/master/app/operations/gimpoperationcolorize.c#L244
    lightness = 2 * target_l - 1
    if lightness > 0:
        result_l = base_l * (1 - lightness) + 1 - (1 - lightness)
    else:
        result_l = base_l * (lightness + 1)
    return target_h, result_l, target_s


rgb_colors = [
    (165, 125, 66),  # brown
    (123, 111, 99),  # ash
    (66, 130, 115),  # marin
    (222, 207, 66),  # yellow
    (198, 77, 107),  # pink
    (82, 106, 74),  # green
    (132, 56, 132),  # purple
    (239, 227, 189),  # white
    (63, 57, 45),  # black
]

hls_colors = [colorsys.rgb_to_hls(x[0] / 256.0, x[1] / 256.0, x[2] / 256.0) for x in rgb_colors]

num_entries = 9
skip = 4
size = (num_entries - skip) * len(rgb_colors) * 2

with open("enemy_palette.bin", 'rb') as infile:
    rgb_1555 = [bitstruct.unpack('u1u5u5u5', bitstruct.byteswap('21', infile.read(2))) for _ in range(0, num_entries)][
               skip:]
    base_palette_rgb = [(r / 32.0, g / 32.0 + gl / 64.0, b / 32.0) for gl, r, g, b in rgb_1555]
    base_palette_hls = [colorsys.rgb_to_hls(r, g, b) for r, g, b in base_palette_rgb]
    with open("enemy_pal.c", 'w') as outfile:
        outfile.write("unsigned char enemy_pal[" + str(size) + "] = {\n")
        for target_hls in hls_colors:
            for base_hls in base_palette_hls:
                colorized_hls = colorize(base_hls, target_hls)
                h, l, s = colorized_hls
                colorized_rgb = colorsys.hls_to_rgb(h, l, s)
                r, g, b = colorized_rgb
                high, low = bitstruct.unpack('u8u8', bitstruct.pack('u1u5u5u5', (g * 64) % 2, r * 32, g * 32, b * 32))
                outfile.write(
                    "\t{}, {}, // rgb({}, {}, {})\n".format(low, high, int(r * 256), int(g * 256), int(b * 256)))
        outfile.write("};\n")

with open("enemy_pal.h", 'w') as header:
    header.write("#ifndef H_ENEMY_PALETTE\n")
    header.write("#define H_ENEMY_PALETTE\n")
    header.write("#define sizeof_enemy_pal " + str(size) + "\n")
    header.write("extern unsigned char enemy_pal[" + str(size) + "];\n")
    header.write("#endif\n")
