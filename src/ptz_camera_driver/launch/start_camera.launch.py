import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    pkg_share = get_package_share_directory('ptz_camera_driver')
    #Costruisce il percorso completo
    config_file_path = os.path.join(pkg_share, 'config', 'ptz_params.yaml')

    # Definisce il nodo da avviare
    start_driver_node = Node(
       package='ptz_camera_driver',
       # Questo è il nome dell'eseguibile definito in CMakeLists.txt
       executable='ptz_camera_app',
       # Questo è il nome ROS del nodo
       name='ptz_camera_driver',
       output='screen',
       #  Passo il file di configurazione al nodo
       parameters=[config_file_path]
    )

    #  Restituisce la descrizione di avvio
    return LaunchDescription([
        start_driver_node
    ])