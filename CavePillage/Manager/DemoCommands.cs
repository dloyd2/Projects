using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DemoCommands : MonoBehaviour
{
    GameObject attack;
    GameObject defense;
    GameObject health;
    GameObject stamina;
    GameObject spawned;
    [SerializeField] GameObject player;
    Vector3 temp;
    void Awake()
    {
        attack = Resources.Load("PickUps/AttackPickup") as GameObject;
        defense = Resources.Load("PickUps/DefensePickup") as GameObject;
        health = Resources.Load("PickUps/HealthPickup") as GameObject;
        stamina = Resources.Load("PickUps/StaminaPickup") as GameObject;

    }
    // Update is called once per frame
    void Update()
    {
        if(Input.GetKeyDown(KeyCode.Alpha1))
        {
            spawned = Instantiate(health);
            temp = player.transform.position;
            temp.y += 1.5f;
            spawned.transform.position = temp;
        }
        else if(Input.GetKeyDown(KeyCode.Alpha2))
        {
            spawned = Instantiate(attack);
            temp = player.transform.position;
            temp.y += 1.5f;
            spawned.transform.position = temp;
        }
        else if(Input.GetKeyDown(KeyCode.Alpha3))
        {
            spawned = Instantiate(defense);
            temp = player.transform.position;
            temp.y += 1.5f;
            spawned.transform.position = temp;
        }
        else if(Input.GetKeyDown(KeyCode.Alpha4))
        {
            spawned = Instantiate(stamina);
            temp = player.transform.position;
            temp.y += 1.5f;
            spawned.transform.position = temp;
        }
        
    }
}
