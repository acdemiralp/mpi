### MPI
Modern C++20 message passing interface wrapper.

### Usage Notes
- Define `MPI_USE_LATEST` to define the latest additions to the MPI 4.0 specification which may not yet be available to the majority of implementations.
- Define `MPI_USE_EXCEPTIONS` to check the return values of all viable functions against `MPI_SUCCESS` and throw an exception otherwise.
- Define `MPI_USE_RELAXED_TRAITS` to prevent the library from checking the types of aggregate elements and triggering static asserts for non-aggregates.
- Compliant types (satisfying `mpi::is_compliant`) are types whose corresponding `mpi::data_type` can be automatically generated:
  - Arithmetic types (satisfying `std::is_arithmetic`), enumerations (satisfying `std::is_enum`), specializations of `std::complex` are compliant types.
  - C-style arrays, `std::array`, `std::pair`, `std::tuple`, and aggregate types (satisfying `std::is_aggregate`) consisting of other compliant types are also compliant types.
  - If your type is none of the above, you can specialize `template <> struct mpi::type_traits<TYPE> { static data_type get_data_type() { return DATA_TYPE; } };` manually.
- The MPI functions accepting buffers may be used with:
  - Compliant types.
  - Contiguous sequential containers (i.e. `std::span`, `std::valarray`, `std::vector<!bool>`) of compliant types.
- Extension functions (starting with MPIX) are not included as they are often implementation-specific. You can nevertheless use them with the wrapper via the native handle getters.

### Design Notes
- Constructors:
  - Copy constructors are deleted unless MPI provides duplication functions (ending with `_dup`) for the object.
  - Move constructors are available whenever possible.
  - The object wrappers have two types of constructors:
    - Managed   constructors: Construct a  new      MPI object, and     be responsible for its destruction.
    - Unmanaged constructors: Accept    an existing MPI object, and not be responsible for its destruction.
- Structures:
  - Structs are for POD data types. They may only contain member variables, constructors, destructors and assignment operators.
- Statics:
  - Namespace functions are preferred over static member functions whenever possible.
- Reflection:
  - The data type for aggregates are created automatically through reflection.
  - Survey of reflection libraries:
    | Library                                            | Built | Extra step | Manual | Dependencies |
    |----------------------------------------------------|-------|------------|--------|--------------|
    | https://github.com/rttrorg/rttr                    | True  | False      | True   | False        |
    | https://github.com/boostorg/pfr                    | False | False      | False  | False        |
    | https://github.com/apolukhin/pfr_non_boost         | False | False      | False  | False        |
    | https://github.com/felixguendling/cista            | False | False      | False  | False        |
    | https://github.com/veselink1/refl-cpp              | False | False      | True   | False        |
    | https://github.com/AustinBrunkhorst/CPP-Reflection | True  | True       | True   | True         |
    | https://github.com/billyquith/ponder               | True  | False      | True   | False        |
    | https://github.com/skypjack/meta                   | False | False      | True   | False        |
    | https://github.com/Ubpa/USRefl                     | False | False      | True   | False        |
    | https://github.com/Ubpa/UDRefl                     | True  | False      | True   | False        |
    | https://github.com/garbageslam/visit_struct        | False | False      | True   | False        |
    | https://github.com/chakaz/reflang                  | True  | True       | False  | True         |
    | https://github.com/Manu343726/tinyrefl             | False | True       | False  | False        |
    | https://github.com/Manu343726/siplasplas           | True  | True       | False  | False        |
    | https://github.com/preshing/FlexibleReflection     | False | False      | True   | False        |
    | https://github.com/Celtoys/clReflect               | True  | True       | True   | False        |
    | https://github.com/Leandros/metareflect            | False | True       | True   | True         |
    | https://github.com/Cylix/Reflex                    | False | False      | True   | True         |
    | https://github.com/boostorg/describe               | False | False      | True   | True         |
  - Decided on PFR after watching https://www.youtube.com/watch?v=abdeAew3gmQ, specifically https://github.com/apolukhin/pfr_non_boost without the boost namespace. 
  - Limitations:
    - The type must be an aggregate (satisfy `std::is_aggregate<type>`): 
      - No user-declared or inherited constructors.
      - No private or protected direct non-static data members.
      - No virtual functions.
      - No virtual, private or protected base classes.
    - Additionally:
      - No const fields.
      - No references.
      - Static data members are ignored.
    - See https://www.boost.org/doc/libs/1_76_0/doc/html/boost_pfr/limitations_and_configuration.html for further detail.

### Coverage (list from https://www.open-mpi.org/doc/v4.1/)
- [x] Constants              
- [x] MPI_Abort                      
- [ ] MPI_Accumulate                 
- [x] MPI_Add_error_class            
- [x] MPI_Add_error_code             
- [x] MPI_Add_error_string           
- [x] ~~MPI_Address~~                  
- [x] MPI_Aint_add                   
- [x] MPI_Aint_diff                  
- [x] MPI_Allgather                  
- [ ] MPI_Allgatherv                 
- [x] MPI_Alloc_mem                  
- [ ] MPI_Allreduce                  
- [ ] MPI_Alltoall                   
- [ ] MPI_Alltoallv                  
- [ ] MPI_Alltoallw                  
- [x] ~~MPI_Attr_delete~~                
- [x] ~~MPI_Attr_get~~                   
- [x] ~~MPI_Attr_put~~                   
- [x] MPI_Barrier                    
- [x] MPI_Bcast                      
- [x] MPI_Bsend                      
- [ ] MPI_Bsend_init                 
- [x] MPI_Buffer_attach              
- [x] MPI_Buffer_detach              
- [x] MPI_Cancel                     
- [x] MPI_Cart_coords                
- [x] MPI_Cart_create                
- [x] MPI_Cart_get                   
- [x] MPI_Cart_map                   
- [x] MPI_Cart_rank                  
- [x] MPI_Cart_shift                 
- [x] MPI_Cart_sub                   
- [x] MPI_Cartdim_get                
- [x] MPI_Close_port                 
- [x] MPI_Comm_accept                
- [x] ~~MPI_Comm_c2f~~                   
- [x] MPI_Comm_call_errhandler       
- [x] MPI_Comm_compare               
- [x] MPI_Comm_connect               
- [x] MPI_Comm_create                
- [x] MPI_Comm_create_errhandler     
- [x] MPI_Comm_create_group          
- [x] MPI_Comm_create_keyval         
- [x] MPI_Comm_delete_attr           
- [x] MPI_Comm_disconnect            
- [x] MPI_Comm_dup                   
- [x] MPI_Comm_dup_with_info         
- [x] ~~MPI_Comm_f2c~~                   
- [x] MPI_Comm_free                  
- [x] MPI_Comm_free_keyval           
- [x] MPI_Comm_get_attr              
- [x] MPI_Comm_get_errhandler        
- [x] MPI_Comm_get_info              
- [x] MPI_Comm_get_name              
- [x] MPI_Comm_get_parent            
- [x] MPI_Comm_group                 
- [x] MPI_Comm_idup                  
- [x] MPI_Comm_join                  
- [x] MPI_Comm_rank                  
- [x] MPI_Comm_remote_group          
- [x] MPI_Comm_remote_size           
- [x] MPI_Comm_set_attr              
- [x] MPI_Comm_set_errhandler        
- [x] MPI_Comm_set_info              
- [x] MPI_Comm_set_name              
- [x] MPI_Comm_size                  
- [x] MPI_Comm_spawn                 
- [x] MPI_Comm_spawn_multiple        
- [x] MPI_Comm_split                 
- [x] MPI_Comm_split_type            
- [x] MPI_Comm_test_inter            
- [ ] MPI_Compare_and_swap           
- [x] MPI_Dims_create                
- [x] MPI_Dist_graph_create          
- [x] MPI_Dist_graph_create_adjacent 
- [x] MPI_Dist_graph_neighbors       
- [x] MPI_Dist_graph_neighbors_count 
- [x] ~MPI_Errhandler_create~
- [x] MPI_Errhandler_free            
- [x] ~~MPI_Errhandler_get~~             
- [x] ~~MPI_Errhandler_set~~            
- [x] MPI_Error_class                
- [x] MPI_Error_string               
- [ ] MPI_Exscan                     
- [ ] MPI_Fetch_and_op               
- [x] ~~MPI_File_c2f~~
- [x] MPI_File_call_errhandler     
- [x] MPI_File_close               
- [x] MPI_File_create_errhandler   
- [x] MPI_File_delete              
- [x] ~~MPI_File_f2c~~                 
- [x] MPI_File_get_amode           
- [x] MPI_File_get_atomicity       
- [x] MPI_File_get_byte_offset     
- [x] MPI_File_get_errhandler      
- [x] MPI_File_get_group           
- [x] MPI_File_get_info            
- [x] MPI_File_get_position        
- [x] MPI_File_get_position_shared 
- [x] MPI_File_get_size            
- [ ] MPI_File_get_type_extent     
- [ ] MPI_File_get_view            
- [ ] MPI_File_iread               
- [ ] MPI_File_iread_all           
- [ ] MPI_File_iread_at            
- [ ] MPI_File_iread_at_all        
- [ ] MPI_File_iread_shared        
- [ ] MPI_File_iwrite              
- [ ] MPI_File_iwrite_all          
- [ ] MPI_File_iwrite_at           
- [ ] MPI_File_iwrite_at_all       
- [ ] MPI_File_iwrite_shared       
- [x] MPI_File_open                
- [ ] MPI_File_preallocate
- [x] MPI_File_read                
- [x] MPI_File_read_all            
- [ ] MPI_File_read_all_begin      
- [ ] MPI_File_read_all_end        
- [ ] MPI_File_read_at             
- [ ] MPI_File_read_at_all         
- [ ] MPI_File_read_at_all_begin   
- [ ] MPI_File_read_at_all_end     
- [ ] MPI_File_read_ordered        
- [ ] MPI_File_read_ordered_begin  
- [ ] MPI_File_read_ordered_end    
- [ ] MPI_File_read_shared         
- [ ] MPI_File_seek                
- [ ] MPI_File_seek_shared         
- [x] MPI_File_set_atomicity       
- [x] MPI_File_set_errhandler      
- [x] MPI_File_set_info            
- [x] MPI_File_set_size            
- [ ] MPI_File_set_view            
- [x] MPI_File_sync                
- [ ] MPI_File_write               
- [ ] MPI_File_write_all           
- [ ] MPI_File_write_all_begin     
- [ ] MPI_File_write_all_end       
- [ ] MPI_File_write_at            
- [ ] MPI_File_write_at_all        
- [ ] MPI_File_write_at_all_begin  
- [ ] MPI_File_write_at_all_end    
- [ ] MPI_File_write_ordered       
- [ ] MPI_File_write_ordered_begin 
- [ ] MPI_File_write_ordered_end   
- [ ] MPI_File_write_shared        
- [x] MPI_Finalize                 
- [x] MPI_Finalized                
- [x] MPI_Free_mem                 
- [x] MPI_Gather                   
- [ ] MPI_Gatherv                  
- [ ] MPI_Get                      
- [ ] MPI_Get_accumulate           
- [x] MPI_Get_address              
- [x] MPI_Get_count                
- [x] MPI_Get_elements             
- [x] MPI_Get_elements_x           
- [x] MPI_Get_library_version      
- [x] MPI_Get_processor_name       
- [x] MPI_Get_version              
- [x] MPI_Graph_create             
- [x] MPI_Graph_get                
- [x] MPI_Graph_map                
- [x] MPI_Graph_neighbors          
- [x] MPI_Graph_neighbors_count    
- [x] MPI_Graphdims_get            
- [x] MPI_Grequest_complete        
- [x] MPI_Grequest_start           
- [x] ~~MPI_Group_c2f~~                
- [x] MPI_Group_compare            
- [x] MPI_Group_difference         
- [x] MPI_Group_excl               
- [x] ~~MPI_Group_f2c~~                
- [x] MPI_Group_free               
- [x] MPI_Group_incl               
- [x] MPI_Group_intersection       
- [x] MPI_Group_range_excl         
- [x] MPI_Group_range_incl         
- [x] MPI_Group_rank               
- [x] MPI_Group_size               
- [x] MPI_Group_translate_ranks    
- [x] MPI_Group_union              
- [x] MPI_Iallgather               
- [ ] MPI_Iallgatherv              
- [ ] MPI_Iallreduce               
- [ ] MPI_Ialltoall                
- [ ] MPI_Ialltoallv               
- [ ] MPI_Ialltoallw               
- [x] MPI_Ibarrier                 
- [x] MPI_Ibcast                   
- [x] MPI_Ibsend                   
- [ ] MPI_Iexscan                  
- [x] MPI_Igather                  
- [ ] MPI_Igatherv                 
- [x] MPI_Improbe                  
- [x] MPI_Imrecv           
- [ ] MPI_Ineighbor_allgather       
- [ ] MPI_Ineighbor_allgatherv      
- [ ] MPI_Ineighbor_alltoall        
- [ ] MPI_Ineighbor_alltoallv       
- [ ] MPI_Ineighbor_alltoallw       
- [x] ~~MPI_Info_c2f~~                  
- [x] MPI_Info_create               
- [x] MPI_Info_delete               
- [x] MPI_Info_dup                  
- [x] MPI_Info_env                  
- [x] ~~MPI_Info_f2c~~                  
- [x] MPI_Info_free                 
- [x] MPI_Info_get                  
- [x] MPI_Info_get_nkeys            
- [x] MPI_Info_get_nthkey           
- [x] MPI_Info_get_valuelen         
- [x] MPI_Info_set                  
- [x] MPI_Init                      
- [x] MPI_Init_thread               
- [x] MPI_Initialized               
- [x] MPI_Intercomm_create          
- [x] MPI_Intercomm_merge           
- [x] MPI_Iprobe                    
- [x] MPI_Irecv                     
- [ ] MPI_Ireduce                   
- [ ] MPI_Ireduce_scatter           
- [ ] MPI_Ireduce_scatter_block     
- [x] MPI_Irsend                    
- [x] MPI_Is_thread_main            
- [ ] MPI_Iscan                     
- [x] MPI_Iscatter                  
- [ ] MPI_Iscatterv                 
- [x] MPI_Isend                     
- [x] MPI_Issend                    
- [x] ~~MPI_Keyval_create~~             
- [x] ~~MPI_Keyval_free~~               
- [x] MPI_Lookup_name               
- [x] ~~MPI_Message_c2f~~               
- [x] ~~MPI_Message_f2c~~               
- [x] MPI_Mprobe                    
- [x] MPI_Mrecv                     
- [ ] MPI_Neighbor_allgather        
- [ ] MPI_Neighbor_allgatherv       
- [ ] MPI_Neighbor_alltoall         
- [ ] MPI_Neighbor_alltoallv        
- [ ] MPI_Neighbor_alltoallw        
- [x] ~~MPI_Op_c2f~~                    
- [x] MPI_Op_commutative            
- [x] MPI_Op_create                 
- [x] ~~MPI_Op_f2c~~                    
- [x] MPI_Op_free                   
- [x] MPI_Open_port                 
- [ ] MPI_Pack                      
- [ ] MPI_Pack_external             
- [ ] MPI_Pack_external_size        
- [ ] MPI_Pack_size                 
- [x] MPI_Pcontrol                  
- [x] MPI_Probe                     
- [x] MPI_Publish_name              
- [ ] MPI_Put                       
- [x] MPI_Query_thread              
- [ ] MPI_Raccumulate               
- [x] MPI_Recv                      
- [ ] MPI_Recv_init                 
- [ ] MPI_Reduce                    
- [ ] MPI_Reduce_local              
- [ ] MPI_Reduce_scatter            
- [ ] MPI_Reduce_scatter_block      
- [ ] MPI_Register_datarep          
- [x] ~~MPI_Request_c2f~~               
- [x] ~~MPI_Request_f2c~~               
- [x] MPI_Request_free              
- [x] MPI_Request_get_status        
- [ ] MPI_Rget                      
- [ ] MPI_Rget_accumulate           
- [ ] MPI_Rput                      
- [x] MPI_Rsend                     
- [ ] MPI_Rsend_init                
- [ ] MPI_Scan                      
- [x] MPI_Scatter                   
- [ ] MPI_Scatterv                  
- [x] MPI_Send                      
- [ ] MPI_Send_init                 
- [x] MPI_Sendrecv                  
- [x] MPI_Sendrecv_replace          
- [x] ~~MPI_Sizeof~~                    
- [x] MPI_Ssend                     
- [ ] MPI_Ssend_init                
- [x] MPI_Start                     
- [x] MPI_Startall                  
- [x] ~~MPI_Status_c2f~~                
- [x] ~~MPI_Status_f2c~~                
- [x] MPI_Status_set_cancelled      
- [x] MPI_Status_set_elements       
- [x] MPI_Status_set_elements_x     
- [x] MPI_T_category_changed        
- [x] MPI_T_category_get_categories 
- [x] MPI_T_category_get_cvars      
- [x] MPI_T_category_get_info       
- [x] MPI_T_category_get_num        
- [x] MPI_T_category_get_pvars      
- [x] MPI_T_cvar_get_info           
- [x] MPI_T_cvar_get_num            
- [x] MPI_T_cvar_handle_alloc       
- [x] MPI_T_cvar_handle_free        
- [x] MPI_T_cvar_read               
- [x] MPI_T_cvar_write              
- [x] MPI_T_enum_get_info           
- [x] MPI_T_enum_get_item  
- [x] MPI_T_finalize 
- [x] MPI_T_init_thread
- [x] MPI_T_pvar_get_info
- [x] MPI_T_pvar_get_num
- [x] MPI_T_pvar_handle_alloc
- [x] MPI_T_pvar_handle_free
- [x] MPI_T_pvar_read
- [x] MPI_T_pvar_readreset
- [x] MPI_T_pvar_reset
- [x] MPI_T_pvar_session_create
- [x] MPI_T_pvar_session_free
- [x] MPI_T_pvar_start
- [x] MPI_T_pvar_stop
- [x] MPI_T_pvar_write
- [x] MPI_Test
- [x] MPI_Test_cancelled
- [x] MPI_Testall
- [x] MPI_Testany
- [x] MPI_Testsome
- [x] MPI_Topo_test
- [x] ~~MPI_Type_c2f~~
- [x] MPI_Type_commit
- [x] MPI_Type_contiguous
- [x] MPI_Type_create_darray
- [x] ~~MPI_Type_create_f90_complex~~
- [x] ~~MPI_Type_create_f90_integer~~
- [x] ~~MPI_Type_create_f90_real~~
- [x] MPI_Type_create_hindexed
- [x] MPI_Type_create_hindexed_block
- [x] MPI_Type_create_hvector
- [x] MPI_Type_create_indexed_block
- [x] MPI_Type_create_keyval
- [x] MPI_Type_create_resized
- [x] MPI_Type_create_struct
- [x] MPI_Type_create_subarray
- [x] MPI_Type_delete_attr
- [x] MPI_Type_dup
- [x] ~~MPI_Type_extent~~
- [x] ~~MPI_Type_f2c~~
- [x] MPI_Type_free
- [x] MPI_Type_free_keyval
- [x] MPI_Type_get_attr
- [x] MPI_Type_get_contents
- [x] MPI_Type_get_envelope
- [x] MPI_Type_get_extent
- [x] MPI_Type_get_extent_x
- [x] MPI_Type_get_name
- [x] MPI_Type_get_true_extent
- [x] MPI_Type_get_true_extent_x
- [x] ~~MPI_Type_hindexed~~
- [x] ~~MPI_Type_hvector~~
- [x] MPI_Type_indexed
- [x] ~~MPI_Type_lb~~
- [ ] MPI_Type_match_size
- [x] MPI_Type_set_attr
- [x] MPI_Type_set_name
- [x] MPI_Type_size
- [x] MPI_Type_size_x
- [x] ~~MPI_Type_struct~~
- [x] ~~MPI_Type_ub~~
- [x] MPI_Type_vector
- [ ] MPI_Unpack
- [ ] MPI_Unpack_external
- [x] MPI_Unpublish_name
- [x] MPI_Wait
- [x] MPI_Waitall
- [x] MPI_Waitany
- [x] MPI_Waitsome
- [x] MPI_Win_allocate
- [x] MPI_Win_allocate_shared
- [x] MPI_Win_attach
- [x] ~~MPI_Win_c2f~~
- [x] MPI_Win_call_errhandler
- [x] MPI_Win_complete
- [x] MPI_Win_create
- [x] MPI_Win_create_dynamic
- [x] MPI_Win_create_errhandler
- [x] MPI_Win_create_keyval
- [x] MPI_Win_delete_attr
- [x] MPI_Win_detach
- [x] ~~MPI_Win_f2c~~
- [x] MPI_Win_fence
- [x] MPI_Win_flush
- [x] MPI_Win_flush_all
- [x] MPI_Win_flush_local
- [x] MPI_Win_flush_local_all
- [x] MPI_Win_free
- [x] MPI_Win_free_keyval
- [x] MPI_Win_get_attr
- [x] MPI_Win_get_errhandler
- [x] MPI_Win_get_group
- [x] MPI_Win_get_info
- [x] MPI_Win_get_name
- [x] MPI_Win_lock
- [x] MPI_Win_lock_all
- [x] MPI_Win_post
- [x] MPI_Win_set_attr
- [x] MPI_Win_set_errhandler
- [x] MPI_Win_set_info
- [x] MPI_Win_set_name
- [x] MPI_Win_shared_query
- [x] MPI_Win_start
- [x] MPI_Win_sync
- [x] MPI_Win_test
- [x] MPI_Win_unlock
- [x] MPI_Win_unlock_all
- [x] MPI_Win_wait
- [x] MPI_Wtick
- [x] MPI_Wtime
- [ ] MPI_Allgather_init
- [ ] MPI_Allgatherv_init
- [ ] MPI_Allreduce_init
- [ ] MPI_Alltoall_init
- [ ] MPI_Alltoallv_init
- [ ] MPI_Alltoallw_init
- [ ] MPI_Barrier_init
- [ ] MPI_Bcast_init
- [x] MPI_Comm_create_from_group
- [x] MPI_Comm_idup_with_info
- [ ] MPI_Exscan_init
- [ ] MPI_Gather_init
- [ ] MPI_Gatherv_init
- [ ] MPI_Group_from_session_pset
- [x] MPI_Info_create_env
- [x] MPI_Info_get_string
- [x] MPI_Intercomm_create_from_groups
- [x] MPI_Isendrecv
- [x] MPI_Isendrecv_replace   
- [ ] MPI_Neighbor_allgather_init
- [ ] MPI_Neighbor_allgatherv_init
- [ ] MPI_Neighbor_alltoall_init
- [ ] MPI_Neighbor_alltoallv_init
- [ ] MPI_Neighbor_alltoallw_init
- [ ] MPI_Parrived   
- [ ] MPI_Pready 
- [ ] MPI_Pready_list
- [ ] MPI_Pready_range
- [ ] MPI_Precv_init
- [ ] MPI_Psend_init
- [ ] MPI_Reduce_init
- [ ] MPI_Reduce_scatter_block_init
- [ ] MPI_Reduce_scatter_init
- [ ] MPI_Scan_init
- [ ] MPI_Scatter_init
- [ ] MPI_Scatterv_init
- [x] MPI_Session_call_errhandler
- [x] MPI_Session_create_errhandler
- [x] ~~MPI_Session_c2f~~
- [x] ~~MPI_Session_f2c~~
- [x] MPI_Session_finalize
- [x] MPI_Session_get_errhandler
- [x] MPI_Session_get_info
- [x] MPI_Session_get_nth_pset
- [x] MPI_Session_get_num_psets
- [x] MPI_Session_get_pset_info
- [x] MPI_Session_init
- [x] MPI_Session_set_errhandler
- [ ] MPI_T_category_get_events
- [x] MPI_T_category_get_index
- [ ] MPI_T_category_get_num_events
- [x] MPI_T_cvar_get_index
- [x] MPI_T_pvar_get_index
- [ ] MPI_T_event_callback_get_info
- [ ] MPI_T_event_callback_set_info
- [ ] MPI_T_event_copy
- [ ] MPI_T_event_get_num
- [ ] MPI_T_event_get_info
- [ ] MPI_T_event_get_index
- [ ] MPI_T_event_get_source
- [ ] MPI_T_event_get_timestamp
- [ ] MPI_T_event_handle_alloc
- [ ] MPI_T_event_handle_free
- [ ] MPI_T_event_handle_get_info
- [ ] MPI_T_event_handle_set_info
- [ ] MPI_T_event_read
- [ ] MPI_T_event_register_callback
- [ ] MPI_T_event_set_dropped_handler
- [ ] MPI_T_source_get_info
- [x] MPI_T_source_get_num
- [ ] MPI_T_source_get_timestamp

### NIGHTS WORK
MPI_MAX_PSET_NAME_LEN
MPI_MAX_STRINGTAG_LEN
MPI_ANY_SOURCE                    
MPI_ANY_TAG                       
MPI_APPNUM                        
MPI_ARGVS_NULL                    
MPI_ARGV_NULL                  
MPI_BOTTOM                        
MPI_BSEND_OVERHEAD     
MPI_COMM_DUP_FN              
MPI_COMM_NULL_COPY_FN             
MPI_COMM_NULL_DELETE_FN           
MPI_Copy_function              
MPI_DISPLACEMENT_CURRENT            
MPI_DOUBLE_PRECISION              
MPI_DUP_FN                        
MPI_Delete_function      
MPI_GROUP_EMPTY 
MPI_HOST 
MPI_Handler_function       
MPI_IN_PLACE                   
MPI_IO         
MPI_KEYVAL_INVALID            
MPI_LASTUSEDCODE               
MPI_MAX_INFO_KEY               
MPI_MAX_INFO_VAL                
MPI_MAX_PROCESSOR_NAME                       
MPI_NULL_COPY_FN               
MPI_NULL_DELETE_FN   
MPI_OFFSET
MPI_Offset
MPI_PACKED
MPI_PROC_NULL
MPI_REPLACE
MPI_SOURCE
MPI_STATUSES_IGNORE
MPI_STATUS_IGNORE
MPI_SUBVERSION
MPI_TAG
MPI_TAG_UB
MPI_TYPECLASS_COMPLEX
MPI_TYPECLASS_INTEGER
MPI_TYPECLASS_REAL
MPI_TYPE_DUP_FN
MPI_TYPE_NULL_COPY_FN
MPI_TYPE_NULL_DELETE_FN
MPI_UNDEFINED
MPI_UNDEFINED_RANK
MPI_UNIVERSE_SIZE
MPI_UNWEIGHTED
MPI_User_function
MPI_VERSION
MPI_WEIGHTS_EMPTY
MPI_WIN_BASE
MPI_WIN_CREATE_FLAVOR
MPI_WIN_DISP_UNIT
MPI_WIN_DUP_FN
MPI_WIN_FLAVOR_ALLOCATE
MPI_WIN_FLAVOR_CREATE
MPI_WIN_FLAVOR_DYNAMIC
MPI_WIN_FLAVOR_SHARED
MPI_WIN_MODEL
MPI_WIN_NULL_COPY_FN
MPI_WIN_NULL_DELETE_FN
MPI_WIN_SEPARATE
MPI_WIN_SIZE
MPI_WIN_UNIFIED
MPI_WTIME_IS_GLOBAL

### Future Work
- Tests.