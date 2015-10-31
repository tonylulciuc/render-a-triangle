# render-a-triangle
Code on how to render a triangle
Use either Visual 2012 or Visual 2013 Express, its free, https://www.microsoft.com/en-us/download/details.aspx?id=44914.
To be able to use the code you'll need to download and install 
DirectX (June 2010) SDK, http://www.microsoft.com/en-us/download/details.aspx?id=6812.
Then you'll need to include the following libraries:

	d3d11.lib
	
	d3dx11d.lib
	
	d3dcompiler.lib
	
	dxerr.lib
	
	dxgi.lib

To add these go to:
	 PROJECT -> Properties -> Linker -> Input
	 
		Then add to Additional Dependencies

Next you must include the paths for the Compiler to search in.
So go to:
	PROJECT -> Properties -> VC++ Directories
	
		Then in Executable Directories add path:
		
				C:\..\Microsoft DirectX SDK (June 2010)\Utilities\bin\(x86 or x64)
				
		Then in Include Directories add path:
		
				C:\..\Microsoft DirectX SDK (June 2010)\Include
				
				$(WindowSDK_IncludePath)
				
		Then in Library Directories add path:
		
				C:\..\Microsoft DirectX SDK (June 2010)\lib\(x86 or x64)

Click apply and it should work

NOTE C: \ ..\ is an abbreviation for the actual path 
