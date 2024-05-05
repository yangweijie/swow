# cafile workaround: set cafile for cURL and OpenSSL

param (
    [string]$PhpBin= "php"
)

$scriptPath = Split-Path -parent $MyInvocation.MyCommand.Definition
. "$scriptPath\utils.ps1" -ToolName "cafile" -MaxTry $MaxTry

$phppath = ((Get-Command $PhpBin).Source | Select-String -Pattern '(.+)\\php\.exe').Matches.Groups[1].Value
$inipath = "$phppath\php.ini"

$cafile = "$phppath\ssl\cacert.pem"
$openssl_cafile_ini = "openssl.cafile = $cafile"
$curl_cafile_ini = "curl.cainfo = $cafile"
info ("Append `"$curl_cafile_ini`", `"$openssl_cafile_ini`" to " + $inipath)
$content = "
$curl_cafile_ini
$openssl_cafile_ini

"
$content | Out-File -Append $inipath -Encoding ASCII # for no BOM
