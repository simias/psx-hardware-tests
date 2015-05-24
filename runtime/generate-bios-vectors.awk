BEGIN {
    print ".set mips1"
    print ".set noreorder"
    print ".set noat"
    print ".set nomacro"
    print ""
    print ".text"
    FS = ","
}
/^#/,/^$/ {
    next
}
{
    if (NF == 3) {
	name = "bios_"$1
	vector = $2
	entry = $3

	gsub(/^[ \t]+/, "", name)
	gsub(/^[ \t]+/, "", vector)
	gsub(/^[ \t]+/, "", entry)

	printf("\n")
	printf(".section .text.%s\n", name)
	printf(".global %s\n", name)
	printf(".type %s, function\n", name)
	printf("\n")
	printf("%s:\n", name)
	printf("\tj  %s\n", vector)
	printf("\tli $t1, %s\n", entry)
    }
}
