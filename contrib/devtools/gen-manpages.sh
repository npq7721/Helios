#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

heliosD=${heliosD:-$SRCDIR/heliosd}
heliosCLI=${heliosCLI:-$SRCDIR/helios-cli}
heliosTX=${heliosTX:-$SRCDIR/helios-tx}
heliosQT=${heliosQT:-$SRCDIR/qt/helios-qt}

[ ! -x $heliosD ] && echo "$heliosD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
RVNVER=($($heliosCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for heliosd if --version-string is not set,
# but has different outcomes for helios-qt and helios-cli.
echo "[COPYRIGHT]" > footer.h2m
$heliosD --version | sed -n '1!p' >> footer.h2m

for cmd in $heliosD $heliosCLI $heliosTX $heliosQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${RVNVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${RVNVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m
