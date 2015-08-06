def draw_ascii(tree, file=None, column_width=80):
    """adapted from biopŷthon's draw ascii function"""

    taxa = tree.get_terminals()
    # Some constants for the drawing calculations
    max_label_width = max(len(str(taxon)) for taxon in taxa)
    drawing_width = column_width - max_label_width - 1
    drawing_height = 2 * len(taxa) - 1

    def get_col_positions(tree):
        """Create a mapping of each clade to its column position."""
        depths = tree.depths()
        # If there are no branch lengths, assume unit branch lengths
        if not max(depths.values()):
            depths = tree.depths(unit_branch_lengths=True)
        # Potential drawing overflow due to rounding -- 1 char per tree layer
        fudge_margin = int(math.ceil(math.log(len(taxa), 2)))
        cols_per_branch_unit = ((drawing_width - fudge_margin)
                                / float(max(depths.values())))
        return dict((clade, int(blen * cols_per_branch_unit + 1.0))
                    for clade, blen in depths.items())

    def get_row_positions(tree):
        positions = dict((taxon, 2 * idx) for idx, taxon in enumerate(taxa))

        def calc_row(clade):
            for subclade in clade:
                if subclade not in positions:
                    calc_row(subclade)
            positions[clade] = ((positions[clade.clades[0]] +
                                 positions[clade.clades[-1]]) // 2)

        calc_row(tree.root)
        return positions

    col_positions = get_col_positions(tree)
    row_positions = get_row_positions(tree)
    char_matrix = [[' ' for x in range(drawing_width)]
                   for y in range(drawing_height)]

    def draw_clade(clade, startcol):
        thiscol = col_positions[clade]
        thisrow = row_positions[clade]
        # Draw a horizontal line
        for col in range(startcol, thiscol):
            char_matrix[thisrow][col] = '_'
        if clade.clades:
            # Draw a vertical line
            toprow = row_positions[clade.clades[0]]
            botrow = row_positions[clade.clades[-1]]
            for row in range(toprow + 1, botrow + 1):
                char_matrix[row][thiscol] = '|'
            # NB: Short terminal branches need something to stop rstrip()
            if (col_positions[clade.clades[0]] - thiscol) < 2:
                char_matrix[toprow][thiscol] = ','
            # Draw descendents
            for child in clade:
                draw_clade(child, thiscol + 1)

    draw_clade(tree.root, 0)
    # Print the complete drawing
    for idx, row in enumerate(char_matrix):
        line = ''.join(row).rstrip()
        # Add labels for terminal taxa in the right margin
        if idx % 2 == 0:
            line += ' ' + str(taxa[idx // 2])
        file.write(line + '\n')
    file.write('\n')