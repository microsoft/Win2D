$CertificateFriendlyName = "Win2DTemporaryKey"
$Publisher = "CN=Win2DTemporaryKey"

$cert = New-SelfSignedCertificate -Type Custom `
    -Subject $Publisher `
    -KeyUsage DigitalSignature `
    -FriendlyName $CertificateFriendlyName `
    -CertStoreLocation "Cert:\CurrentUser\My" `
    -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3", "2.5.29.19={text}")

$certificateBytes = $cert.Export([System.Security.Cryptography.X509Certificates.X509ContentType]::Pkcs12)
[System.IO.File]::WriteAllBytes("$PSScriptRoot\Assets\TemporaryKey.pfx", $certificateBytes)

$certThumbprint = $cert.Thumbprint
$tempCertPath = Join-Path $Env:Temp ((New-Guid).ToString() + '.cer')
Export-Certificate -Cert $cert -FilePath $tempCertPath | Out-Null
Remove-Item ('Cert:\CurrentUser\My\' + $certThumbprint)
Start-Process PowerShell -Wait -Verb RunAs "-NoProfile -ExecutionPolicy Bypass -Command `"certutil.exe '-addstore' 'TrustedPeople' '$tempCertPath'; (Get-ChildItem -Path Cert:\LocalMachine\TrustedPeople\$certThumbprint).FriendlyName = '$CertificateFriendlyName';`"";
Remove-Item $tempCertPath