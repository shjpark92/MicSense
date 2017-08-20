set( _DEFAULT_COMPONENTS fftw3 fftw3_threads fftw3f fftw3f_threads fftw3l fftw3l_threads )

if( FFTW3_FIND_COMPONENTS )
	set( _COMPONENTS ${FFTW3_FIND_COMPONENTS} )
else()
	set( _COMPONENTS ${_DEFAULT_COMPONENTS} )
endif()

include( FindPackageHandleStandardArgs )

find_path( FFTW3_INCLUDE_DIR fftw3.h )

foreach( _lib ${_COMPONENTS} )
	string( TOUPPER FFTW3_${_lib} _COMP )
	find_library( ${_COMP}_LIBRARY ${_lib} )
	set(FFTW3_LIBRARIES ${FFTW3_LIBRARIES} ${${_COMP}_LIBRARY})
	if( ${_COMP}_LIBRARY )
		set( ${_COMP}_FOUND TRUE )
	else()
		set( ${_COMP}_FOUND FALSE )
		message( "-- Could NOT find component of FFTW3: ${_lib}" )
	endif()
	mark_as_advanced( ${_COMP}_LIBRARY ${_COMP}_FOUND )
endforeach()

find_package_handle_standard_args( FFTW3 DEFAULT_MSG FFTW3_LIBRARIES FFTW3_INCLUDE_DIR )
mark_as_advanced( FFTW3_INCLUDE_DIR FFTW3_LIBRARIES )
