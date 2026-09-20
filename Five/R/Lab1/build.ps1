$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot
$gcc = 'C:\MinGW\w64devkit\bin\gcc.exe'
$build = Join-Path $root 'build'

New-Item -ItemType Directory -Force -Path $build | Out-Null

$commonArgs = @('-I', (Join-Path $root 'include'), '-I', (Join-Path $root 'Tanki'))
$cSources = @(
    'src/glad.c',
    'Tanki/game.c',
    'Tanki/input.c',
    'Tanki/lighting.c',
    'Tanki/map.c',
    'Tanki/particles.c',
    'Tanki/powerup.c',
    'Tanki/render.c',
    'Tanki/tank.c',
    'Tanki/crypt.c'
)
$objects = @()

foreach ($source in $cSources) {
    $object = Join-Path $build (([IO.Path]::GetFileNameWithoutExtension($source)) + '.o')
    & $gcc '-std=c17' @commonArgs '-c' (Join-Path $root $source) '-o' $object
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    $objects += $object
}

$mainObject = Join-Path $build 'main.o'
& $gcc '-std=c17' @commonArgs '-c' (Join-Path $root 'src/main.c') '-o' $mainObject
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& $gcc @objects $mainObject '-L' (Join-Path $root 'lib') '-lglfw3dll' '-lopengl32' '-o' (Join-Path $root 'tanks.exe')
exit $LASTEXITCODE