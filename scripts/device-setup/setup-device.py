import os
import sys
from pathlib import Path

import requests
import paramiko
import argparse

OWNER = "Gruncan"
REPO = "mem-monitor"
ARTIFACTS = [
    "lib_mem_tracker.so",
    "mem_monitor"
]

def get_release(version=None):
    url = f'https://api.github.com/repos/{OWNER}/{REPO}/releases/'
    if version is None:
        print("Version not specified defaulting, to latest")
        url += 'latest'
    else:
        url += f'tag/{version}'

    response = requests.get(url)
    if response.status_code == 200:
        release = response.json()
        print(f"Found Version: {release['name']} ({release['tag_name']})")
        return release["assets"]
    else:
        print(f"Failed to fetch release: {response.status_code}")
        return None


def download_artifacts(assets):
    artifacts = (asset for asset in assets if asset['name'] in ARTIFACTS)
    for component in artifacts:
        print(f"\t- Downloading {component['name']}...")
        download_url = component['browser_download_url']
        download_response = requests.get(download_url, stream=True)
        full_path = os.path.join(os.getcwd(), component['name'])
        with open(full_path, 'wb') as f:
            for chunk in download_response.iter_content(chunk_size=8192):
                f.write(chunk)
        yield full_path

def transfer_artifacts(sftp_client, artifacts, scripts, remote_path):
    for artifact in artifacts:
        print(f"\t- Transferring {artifact} ... to ... {remote_path}")
        sftp_client.put(artifact, os.path.join(remote_path, Path(artifact).name))

    for script in scripts:
        print(f"\t- Transferring {script} ... to ... {remote_path}")
        sftp_client.put(script, os.path.join(remote_path, Path(script).name))

def pre_setup():
    print("\n\n")
    print("Running pre-setup")

def post_setup():
    print("\n\n")
    print("Running post-setup")



def main():
    parser = argparse.ArgumentParser(description="Setup a device for memory debugging")
    parser.add_argument("-d", "--device", type=str, help="The ip or hostname of the device", required=True)
    parser.add_argument("-u", "--user", type=str, help="The name of remote user to login", required=False)
    parser.add_argument("-p", "--password", type=str, help="The password of remote user to login", required=False)
    parser.add_argument("-st", "--tracker", type=str, help="Tracker script filepath", required=True)
    parser.add_argument("-s", "--start", type=str, help="Start script filepath", required=True)

    args = parser.parse_args()

    try:
        ssh_client = paramiko.SSHClient()
        ssh_client.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh_client.connect(hostname=args.device, port=22, username=args.user, password=args.password)
        print("======================================")
        print(f"Connected to {args.device}")
        print("======================================\n\n")

        if not os.path.isfile(args.tracker):
           raise FileNotFoundError(f"File {args.tracker} not found")
        if not os.path.isfile(args.start):
           raise FileNotFoundError(f"File {args.start} not found")

        print(f"Searching for release...")
        release_assets = get_release()
        artifact_names = download_artifacts(release_assets)

        print("\n\n")
        print("Transferring artifacts to device...")
        transfer_client = ssh_client.open_sftp()
        remote_path = f"/home/{args.user}/"
        transfer_artifacts(transfer_client, artifact_names, [args.tracker, args.start], remote_path)
        transfer_client.close()

        print("Executing script...")
        start_script_path = os.path.join(remote_path, Path(args.start).name)
        ssh_client.exec_command(f"chmod 755 {start_script_path}")
        ssh_client.exec_command(f"{start_script_path}")

    except Exception as e:
        print("Failed to setup device:", str(e), file=sys.stderr)
        sys.exit(1)
    finally:
        ssh_client.close()


if __name__ == '__main__':
    main()