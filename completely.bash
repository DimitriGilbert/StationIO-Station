# StationIO completion                                     -*- shell-script -*-

# This bash completions script was generated by
# completely (https://github.com/dannyben/completely)
# Modifying it manually is not recommended

_StationIO_completions_filter() {
  local words="$1"
  local cur=${COMP_WORDS[COMP_CWORD]}
  local result=()

  if [[ "${cur:0:1}" == "-" ]]; then
    echo "$words"
  
  else
    for word in $words; do
      [[ "${word:0:1}" != "-" ]] && result+=("$word")
    done

    echo "${result[*]}"

  fi
}

_StationIO_completions() {
  local cur=${COMP_WORDS[COMP_CWORD]}
  local compwords=("${COMP_WORDS[@]:1:$COMP_CWORD-1}")
  local compline="${compwords[*]}"

  case "$compline" in
    'build-fs'*'--file-no-gz')
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -A file -- "$cur" )
      ;;

    'build-fs'*'--directory')
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -A directory -- "$cur" )
      ;;

    'create-main'*'--sensor')
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -W "$(_StationIO_completions_filter "$(if [ -d src/sensors ]; then echo $(cd src/sensors && /usr/bin/ls *.h | sed 's/\.h//g') ;fi)")" -- "$cur" )
      ;;

    'create-main'*'--file')
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -A file -- "$cur" )
      ;;

    'build-fs'*'--file')
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -A file -- "$cur" )
      ;;

    'build-fs'*'--css')
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -A file -- "$cur" )
      ;;

    'build-fs'*'--js')
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -A file -- "$cur" )
      ;;

    'read'*'--output')
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -A file -- "$cur" )
      ;;

    'create-main'*)
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -W "$(_StationIO_completions_filter "--sensor -s --timer -t --endpoint -e --wifi-ssid -w --wifi-pass -p --include --declare --setup --loop --node --file")" -- "$cur" )
      ;;

    'build-fs'*)
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -W "$(_StationIO_completions_filter "--js -j --css -c --directory -d --file -f --file-no-gz -F --node --pio --environement -e --upload -u --dry --no-dry -D")" -- "$cur" )
      ;;

    'set-conf'*)
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -W "$(_StationIO_completions_filter "$(if [ -d stations/.stations ]; then /usr/bin/ls stations/.stations ;fi) --sensor -s --ip -i --board -b --data-file -f --data-directory -d --js -j --css -c")" -- "$cur" )
      ;;

    'create'*)
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -W "$(_StationIO_completions_filter "--ip --board -b --config --no-config --edit --no-edit --force --no-force")" -- "$cur" )
      ;;

    'upload'*)
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -W "$(_StationIO_completions_filter "$(if [ -d stations/.stations ]; then /usr/bin/ls stations/.stations ;fi) --ip --port -p --board -b --pio --ota --no-ota --fs --no-fs")" -- "$cur" )
      ;;

    'read'*)
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -W "$(_StationIO_completions_filter "$(if [ -d stations/.stations ]; then /usr/bin/ls stations/.stations ;fi) --sensor -s --format -f --mesure -m --output -o")" -- "$cur" )
      ;;

    *)
      while read -r; do COMPREPLY+=( "$REPLY" ); done < <( compgen -W "$(_StationIO_completions_filter "build-fs create create-main read set-conf upload")" -- "$cur" )
      ;;

  esac
} &&
complete -F _StationIO_completions StationIO

# ex: filetype=sh
