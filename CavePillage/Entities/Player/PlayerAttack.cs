using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;

public class PlayerAttack : MonoBehaviour
{
    public Animator animator;
    BoxCollider2D meleeAttackBox;
    public int meleeDamage = 2;
    public float attackRate = 1f;
    float timer;
    [SerializeField] TextMeshProUGUI attackAmount;
    public void ChangeMeleeDamage(int toAdd)
    {
        meleeDamage += toAdd;
        Debug.Log("current melee damage is now " + meleeDamage);
        attackAmount.text = meleeDamage.ToString();
    }
    void Awake()
    {
        meleeAttackBox = GameObject.FindGameObjectWithTag("PlayerMelee").GetComponent<BoxCollider2D>();
        meleeAttackBox.enabled = false;
        attackAmount.text = meleeDamage.ToString();
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;

        if (Input.GetButtonDown("Fire1") && timer >= attackRate)      //check attacks
        {
            animator.SetBool("attacking", true);
            meleeAttackBox.enabled = true;
        }
        else if(Input.GetButtonUp("Fire1"))
        {
            animator.SetBool("attacking", false);
            meleeAttackBox.enabled = false;
        }
    }
    
    void OnTriggerStay2D(Collider2D other)
    {
        Vector2 temp = new Vector2(-200, 300);
        if(other.gameObject.layer == LayerMask.NameToLayer("Enemies") && timer >= attackRate)
        {
            other.gameObject.SendMessage("TakeDamage", meleeDamage);
            timer = 0f;
        }
    }
    
}
