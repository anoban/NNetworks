module idxio
    use iso_fortran_env
    implicit none
    
    ! define custom type to represent idx1 and idx3 file types
    
    type :: idx1    ! idx1 is a simple binary file format used to store one byte `labels`
        
        ! [offset] [type]          [value]          [description]
        ! 0000     32 bit integer  0x00000801(2049) magic number (MSB first)
        ! 0004     32 bit integer  10000            number of items
        ! 0008     unsigned byte   ??               label
        ! 0009     unsigned byte   ??               label
        ! ........
        ! xxxx     unsigned byte   ??               label
        
        integer(kind = int32) :: magic      ! idx1 magic number, must be 2049
        integer(kind = int32) :: nlabels    ! number of labels in the array downstream
        integer(kind = int8), dimension(:)  :: labels     ! this is an array of nlabels length
        
    end type idx1
    
    
    
    
    type :: idx3    ! idx3 is a binary file format that is used to store image pixels as an array of arrays
        
            ! [offset] [type]          [value]          [description]
            ! 0000     32 bit integer  0x00000803(2051) magic number
            ! 0004     32 bit integer  10000            number of images
            ! 0008     32 bit integer  28               number of rows
            ! 0012     32 bit integer  28               number of columns
            ! 0016     unsigned byte   ??               pixel
            ! 0017     unsigned byte   ??               pixel
            ! ........
            ! xxxx     unsigned byte   ??               pixel
            
            integer(kind = int32) :: magic      ! idx3 magic number, must be 2051
            integer(kind = int32) :: nimages    ! number of images stored in the array donstream
            ! an image can be imagined as a width x height pixel block!
            integer(kind = int32) :: nrows      ! number of rows in each image
            integer(kind = int32) :: ncols      ! number of columns in each image
            integer(kind = int8), dimension(:)  :: labels     ! this is an array of nlabels length 8 bit unsigned integers
        
    end type idx3
    
    
end module idxio    