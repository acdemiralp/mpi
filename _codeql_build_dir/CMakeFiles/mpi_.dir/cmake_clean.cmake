file(REMOVE_RECURSE
  "libmpi_.a"
  "libmpi_.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/mpi_.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
